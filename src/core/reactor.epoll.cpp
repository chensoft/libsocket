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
#include <limits>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = EPOLLET;
const int chen::reactor::FlagOnce = EPOLLONESHOT;

chen::reactor::reactor(int count) : _count(count)
{
    // create epoll file descriptor
    // 1 is just a hint, see http://man7.org/linux/man-pages/man2/epoll_create.2.html
    if ((this->_epoll = ::epoll_create(1)) < 0)
        throw std::system_error(sys::error(), "epoll: failed to create epoll");

    // create eventfd to receive wake message
    this->_wake.handle = ::eventfd(0, 0);

    if ((this->_wake.handle < 0) || ::fcntl(this->_wake.handle, F_SETFL, ::fcntl(this->_wake.handle, F_GETFL, 0) | O_NONBLOCK))
    {
        ::close(this->_epoll);
        throw std::system_error(sys::error(), "epoll: failed to create eventfd");
    }

    this->set(&this->_wake, ModeRead, FlagEdge);
}

chen::reactor::~reactor()
{
    ::close(this->_epoll);
    ::close(this->_wake.handle);
}

// modify
void chen::reactor::set(Event *ev, int mode, int flag)
{
    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events  |= flag | EPOLLRDHUP;
    event.data.ptr = ev;

    // register event
    if (::epoll_ctl(this->_epoll, EPOLL_CTL_MOD, ev->handle, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_epoll, EPOLL_CTL_ADD, ev->handle, &event) != 0))
            throw std::system_error(sys::error(), "epoll: failed to set event");
    }
}

void chen::reactor::del(Event *ev)
{
    if ((::epoll_ctl(this->_epoll, EPOLL_CTL_DEL, ev->handle, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "epoll: failed to delete event");
}

// run
void chen::reactor::run(double timeout)
{
    while (this->once(timeout))
        ;
}

bool chen::reactor::once(double timeout)
{
    // poll events
    ::epoll_event events[this->_count];  // VLA
    int result = ::epoll_wait(this->_epoll, events, this->_count, timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

    if (result <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)  // timeout if result is zero
            return false;
        else
            throw std::system_error(sys::error(), "epoll: failed to poll event");
    }

    // invoke callback
    for (int i = 0; i < result; ++i)
    {
        auto &item = events[i];

        // user request to stop
        if (item.data.ptr == &this->_wake)
        {
            ::eventfd_t dummy;
            ::eventfd_read(this->_wake.handle, &dummy);
            return false;
        }

        // check events, multiple events maybe occur
        auto ev = static_cast<Event*>(item.data.ptr);
        if (!ev->callback)
            continue;

        if ((item.events & EPOLLRDHUP) || (item.events & EPOLLERR) || (item.events & EPOLLHUP))
        {
            ev->callback(Type::Closed);
        }
        else
        {
            if (item.events & EPOLLIN)
                ev->callback(Type::Readable);

            if (item.events & EPOLLOUT)
                ev->callback(Type::Writable);
        }
    }

    return result > 0;
}

void chen::reactor::stop()
{
    // notify wake message via eventfd
    if (::eventfd_write(this->_wake.handle, 1) != 0)
        throw std::system_error(sys::error(), "epoll: failed to wake the epoll");
}

#endif