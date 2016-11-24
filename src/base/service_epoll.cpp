/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/base/service_epoll.hpp>
#include <chen/sys/sys.hpp>
#include <sys/eventfd.h>
#include <limits>

// -----------------------------------------------------------------------------
// epoll
chen::service_epoll::service_epoll()
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

    this->set(this->_ef, &this->_ef, OpcodeRead, FlagEdge);
}

chen::service_epoll::~service_epoll()
{
    ::close(this->_fd);
    ::close(this->_ef);
}

// modify
void chen::service_epoll::set(handle_t fd, void *data, int opcode, int flag)
{
    ::epoll_event event{};

    if (opcode & OpcodeRead)
        event.events |= EPOLLIN;

    if (opcode & OpcodeWrite)
        event.events |= EPOLLOUT;

    event.events  |= flag | EPOLLRDHUP;
    event.data.ptr = data;

    if (::epoll_ctl(this->_fd, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "epoll: failed to set event");
    }
}

void chen::service_epoll::del(handle_t fd)
{
    if ((::epoll_ctl(this->_fd, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "epoll: failed to delete event");
}

// poll
std::size_t chen::service_epoll::poll(std::vector<Data> &cache, std::size_t count, double timeout)
{
    if (!count)
        return 0;

    // reset wake event
    ::eventfd_t wake;
    ::eventfd_read(this->_ef, &wake);

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
        auto insert = [&] (Event code) {
            ++number;

            if (i < origin)
                cache[i] = Data(event.data.ptr, code);
            else
                cache.emplace_back(Data(event.data.ptr, code));
        };

        if ((event.events & EPOLLRDHUP) || (event.events & EPOLLERR) || (event.events & EPOLLHUP))
        {
            insert(Event::Ended);
        }
        else
        {
            if (event.events & EPOLLIN)
                insert(Event::Readable);

            if (event.events & EPOLLOUT)
                insert(Event::Writable);
        }
    }

    return number;
}

std::vector<chen::service_epoll::Data> chen::service_epoll::poll(std::size_t count, double timeout)
{
    std::vector<chen::service_epoll::Data> ret;
    this->poll(ret, count, timeout);
    return ret;
}

void chen::service_epoll::stop()
{
    // notify wake message via eventfd
    if (::eventfd_write(this->_ef, 1) != 0)
        throw std::system_error(sys::error(), "epoll: failed to wake the epoll");
}

#endif