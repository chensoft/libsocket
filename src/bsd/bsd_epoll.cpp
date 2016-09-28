/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/bsd/bsd_epoll.hpp>
#include <chen/sys/sys.hpp>
#include <sys/epoll.h>
#include <unistd.h>

// -----------------------------------------------------------------------------
// epoll
chen::bsd::epoll::epoll()
{
    if ((this->_fd = ::epoll_create1(0)) < 0)
        throw std::system_error(sys::error(), "epoll: failed to create epoll");

    if (::pipe(this->_pp) < 0)
    {
        ::close(this->_fd);
        throw std::system_error(sys::error(), "epoll: failed to create pipe");
    }

    // register pipe to receive exit message
    this->add(this->_pp[0], Opcode::Read);
}

chen::bsd::epoll::~epoll()
{
    ::close(this->_fd);
    ::close(this->_pp[0]);
    ::close(this->_pp[1]);
}

// modify
void chen::bsd::epoll::add(int fd, Opcode opcode, std::uint16_t flag)
{
    struct ::epoll_event event{};

    event.events  = this->opcode(opcode) | EPOLLRDHUP;
    event.data.fd = fd;

    if (flag & FlagOnce)
        event.events |= EPOLLONESHOT;

    if (flag & FlagEdge)
        event.events |= EPOLLET;

    if (::epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) < 0)
        throw std::system_error(chen::sys::error(), "epoll: failed to add event");
}

void chen::bsd::epoll::del(int fd)
{
    if ((::epoll_ctl(this->_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "epoll: failed to delete event");
}

// poll
chen::bsd::epoll::Data chen::bsd::epoll::poll()
{
    struct ::epoll_event event{};

    // poll next event
    if (::epoll_wait(this->_fd, &event, 1, -1) <= 0)
        throw std::system_error(sys::error(), "epoll: failed to poll event");

    // check exit status
    if (event.data.fd == this->_pp[0])
    {
        char dummy;
        ::read(this->_pp[0], &dummy, 1);
        return {};
    }

    // return the data
    Data ret;

    ret.fd = event.data.fd;
    ret.ev = this->event(event.events);

    return ret;
}

void chen::bsd::epoll::exit()
{
    // notify exit message via pipe
    if (::write(this->_pp[1], "\n", 1) != 1)
        throw std::system_error(sys::error(), "epoll: failed to exit the epoll");
}

// misc
std::uint32_t chen::bsd::epoll::opcode(Opcode opcode)
{
    switch (opcode)
    {
        case Opcode::Read:
            return EPOLLIN;

        case Opcode::Write:
            return EPOLLOUT;
    }

    return 0;
}

chen::bsd::epoll::Event chen::bsd::epoll::event(std::uint32_t events)
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