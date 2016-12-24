/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>
#include <sys/eventfd.h>
#include <algorithm>
#include <limits>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = EPOLLET;
const int chen::reactor::FlagOnce = EPOLLONESHOT;

chen::reactor::reactor(std::size_t events) : _events(events)
{
    // create epoll file descriptor
    // 1 is just a hint, see http://man7.org/linux/man-pages/man2/epoll_create.2.html
    if ((this->_epoll = ::epoll_create(1)) < 0)
        throw std::system_error(sys::error(), "epoll: failed to create epoll");

    // create eventfd to receive wake message
    this->_wake = ::eventfd(0, 0);

    if ((this->_wake < 0) || ::fcntl(this->_wake, F_SETFL, ::fcntl(this->_wake, F_GETFL, 0) | O_NONBLOCK))
    {
        ::close(this->_epoll);
        throw std::system_error(sys::error(), "epoll: failed to create eventfd");
    }

    this->set(this->_wake, nullptr, ModeRead, FlagEdge);
}

chen::reactor::~reactor()
{
    ::close(this->_epoll);
    ::close(this->_wake);
}

// modify
void chen::reactor::set(handle_t fd, callback cb, int mode, int flag)
{
    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events |= flag | EPOLLRDHUP;
    event.data.fd = fd;

    // register event
    if (::epoll_ctl(this->_epoll, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_epoll, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "epoll: failed to set event");
    }

    // register callback
    this->_callbacks[fd] = cb;
}

void chen::reactor::del(handle_t fd)
{
    // delete callback
    this->_callbacks.erase(fd);

    // disable pending
    std::for_each(this->_events.begin(), this->_events.end(), [=] (::epoll_event &event) {
        // user may delete fd in previous callback, if there are events to be
        // processed will lead to errors, so we disable unhandled events here
        if (event.data.fd == fd)
            event.data.fd = invalid_handle;
    });

    // delete event
    if ((::epoll_ctl(this->_epoll, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "epoll: failed to delete event");
}

// run
void chen::reactor::run(double timeout)
{
    while (!this->once(timeout))
        ;
}

bool chen::reactor::once(double timeout)
{
    // poll events
    int result = ::epoll_wait(this->_epoll, this->_events.data(), static_cast<int>(this->_events.size()), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

    if (result <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)  // timeout if result is zero
            return false;
        else
            throw std::system_error(sys::error(), "epoll: failed to poll event");
    }

    // invoke callback
    for (std::size_t i = 0; i < result; ++i)
    {
        auto &event = this->_events[i];
        auto handle = event.data.fd;

        // disabled by user
        if (handle == invalid_handle)
            continue;

        // user request to stop
        if (handle == this->_wake)
        {
            ::eventfd_t dummy;
            ::eventfd_read(this->_wake, &dummy);
            return false;
        }

        // check events, multiple events maybe occur
        auto method = this->_callbacks.find(handle);
        auto invoke = [&] (Type type) {
            method->second(handle, type);
        };

        if (method == this->_callbacks.end())
            continue;

        if ((event.events & EPOLLRDHUP) || (event.events & EPOLLERR) || (event.events & EPOLLHUP))
        {
            invoke(Type::Closed);
        }
        else
        {
            if (event.events & EPOLLIN)
                invoke(Type::Readable);

            if (event.events & EPOLLOUT)
                invoke(Type::Writable);
        }
    }

    return result > 0;
}

void chen::reactor::stop()
{
    // notify wake message via eventfd
    if (::eventfd_write(this->_wake, 1) != 0)
        throw std::system_error(sys::error(), "epoll: failed to wake the epoll");
}

#endif