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

chen::reactor::reactor()
{
    // create epoll file descriptor
    // 1 is just a hint, see http://man7.org/linux/man-pages/man2/epoll_create.2.html
    if ((this->_fd = ::epoll_create(1)) < 0)
        throw std::system_error(sys::error(), "epoll: failed to create epoll");

    // create eventfd to receive wake message
    this->_ef = ::eventfd(0, 0);

    if ((this->_ef < 0) || ::fcntl(this->_ef, F_SETFL, ::fcntl(this->_ef, F_GETFL, 0) | O_NONBLOCK))
    {
        ::close(this->_fd);
        throw std::system_error(sys::error(), "epoll: failed to create eventfd");
    }

    this->set(this->_ef, &this->_ef, ModeRead, FlagEdge);
}

chen::reactor::~reactor()
{
    ::close(this->_fd);
    ::close(this->_ef);
}

// modify
void chen::reactor::set(handle_t fd, void *data, int mode, int flag)
{
    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events  |= flag | EPOLLRDHUP;
    event.data.ptr = data;

    if (::epoll_ctl(this->_fd, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "epoll: failed to set event");
    }
}

void chen::reactor::del(handle_t fd)
{
    if ((::epoll_ctl(this->_fd, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "epoll: failed to delete event");
}

// run
void chen::reactor::stop()
{
    // notify wake message via eventfd
    if (::eventfd_write(this->_ef, 1) != 0)
        throw std::system_error(sys::error(), "epoll: failed to wake the epoll");
}

// poll
std::vector<chen::reactor::Data> chen::reactor::poll(std::size_t count, double timeout)
{
    std::vector<chen::reactor::Data> ret;
    this->poll(ret, count, timeout);
    return ret;
}

std::size_t chen::reactor::poll(std::vector<Data> &cache, std::size_t count, double timeout)
{
    if (!count)
        return 0;

    // poll next events
    ::epoll_event events[count];  // VLA
    int result = ::epoll_wait(this->_fd, events, static_cast<int>(count), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

    if (result <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)  // timeout if result is zero
            return 0;
        else
            throw std::system_error(sys::error(), "epoll: failed to poll event");
    }

    // collect poll data
    std::size_t origin = cache.size();
    std::size_t number = 0;

    for (std::size_t i = 0; i < result; ++i)
    {
        auto &event = events[i];

        // user request to stop
        if (event.data.ptr == &this->_ef)
        {
            ::eventfd_t dummy;
            ::eventfd_read(this->_ef, &dummy);
            return 0;
        }

        // check events, multiple events maybe occur
        auto insert = [&] (Type type) {
            ++number;

            if (i < origin)
                cache[i] = Data(event.data.ptr, type);
            else
                cache.emplace_back(Data(event.data.ptr, type));
        };

        if ((event.events & EPOLLRDHUP) || (event.events & EPOLLERR) || (event.events & EPOLLHUP))
        {
            insert(Type::Closed);
        }
        else
        {
            if (event.events & EPOLLIN)
                insert(Type::Readable);

            if (event.events & EPOLLOUT)
                insert(Type::Writable);
        }
    }

    return number;
}

#endif