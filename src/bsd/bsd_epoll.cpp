/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/bsd/bsd_epoll.hpp>
#include <socket/config.hpp>
#include <chen/sys/sys.hpp>
#include <sys/eventfd.h>
#include <sys/epoll.h>

// -----------------------------------------------------------------------------
// epoll
const int chen::bsd::epoll::FlagOnce = EPOLLONESHOT;
const int chen::bsd::epoll::FlagEdge = EPOLLET;

chen::bsd::epoll::epoll()
{
    // create epoll file descriptor
    if ((this->_fd = ::epoll_create1(0)) < 0)
        throw std::system_error(sys::error(), "epoll: failed to create epoll");

    // create eventfd to receive wake message
    if ((this->_ef = ::eventfd(0, 0)) < 0)
    {
        ::close(this->_fd);
        throw std::system_error(sys::error(), "epoll: failed to create eventfd");
    }

    this->set(this->_ef, OpcodeRead, FlagEdge);
}

chen::bsd::epoll::~epoll()
{
    ::close(this->_fd);
    ::close(this->_ef);
}

// modify
void chen::bsd::epoll::set(int fd, int opcode, int flag)
{
    struct ::epoll_event event{};

    if (opcode & OpcodeRead)
        event.events |= EPOLLIN;

    if (opcode & OpcodeWrite)
        event.events |= EPOLLOUT;

    event.events |= flag | EPOLLRDHUP;
    event.data.fd = fd;

    if (::epoll_ctl(this->_fd, EPOLL_CTL_MOD, fd, &event) < 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) < 0))
            throw std::system_error(sys::error(), "epoll: failed to set event");
    }
}

void chen::bsd::epoll::del(int fd)
{
    if ((::epoll_ctl(this->_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "epoll: failed to delete event");
}

// poll
chen::bsd::epoll::Data chen::bsd::epoll::poll()
{
    struct ::epoll_event event{};

    // poll next event
    if (::epoll_wait(this->_fd, &event, 1, -1) <= 0)
        throw std::system_error(sys::error(), "epoll: failed to poll event");

    // check exit status
    if (event.data.fd == this->_ef)
    {
        ::eventfd_t dummy;
        ::eventfd_read(this->_ef, &dummy);
        return {};
    }

    // return the data
    Data ret;

    ret.fd = event.data.fd;
    ret.ev = this->event(event.events);

    return ret;
}

void chen::bsd::epoll::wake()
{
    // notify wake message via eventfd
    if (::eventfd_write(this->_ef, 1) != 0)
        throw std::system_error(sys::error(), "epoll: failed to wake the epoll");
}

// misc
chen::bsd::epoll::Event chen::bsd::epoll::event(unsigned events)
{
    if ((events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP))
        return Event::End;

    if (events & EPOLLIN)
        return Event::Read;
    else if (events & EPOLLOUT)
        return Event::Write;
    else
        throw std::runtime_error("epoll: unknown event detect");
}

#endif