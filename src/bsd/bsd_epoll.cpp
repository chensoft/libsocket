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

    if (::epoll_ctl(this->_fd, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "epoll: failed to set event");
    }
}

void chen::bsd::epoll::del(int fd)
{
    if ((::epoll_ctl(this->_fd, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "epoll: failed to delete event");
}

// poll
chen::bsd::epoll::Data chen::bsd::epoll::poll(double timeout)
{
    auto ret = this->fetch(1, timeout);
    return !ret.empty() ? ret.front() : Data();
}

std::vector<chen::bsd::epoll::Data> chen::bsd::epoll::fetch(int count, double timeout)
{
    if (count <= 0)
        return {};

    struct ::epoll_event events[count];
    int result = 0;

    // poll next events
    if ((result = ::epoll_wait(this->_fd, events, count, timeout < 0 ? -1 : static_cast<int>(timeout * 1000))) < 0)
        throw std::system_error(sys::error(), "epoll: failed to poll event");

    // check if timeout
    std::vector<Data> ret;

    if ((result == 0) && (timeout >= 0))
    {
        ret.emplace_back(Data(-1, Event::Timeout));
        return ret;
    }

    // check return data
    for (int i = 0; i < result; ++i)
    {
        auto &event = events[i];

        if (event.data.fd == this->_ef)
        {
            // user request to exit
            ::eventfd_t dummy;
            ::eventfd_read(this->_ef, &dummy);
            return {};
        }
        else
        {
            ret.emplace_back(Data(event.data.fd, this->event(event.events)));
        }
    }

    return ret;
}

void chen::bsd::epoll::exit()
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