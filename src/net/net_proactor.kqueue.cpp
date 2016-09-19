/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/net/net_proactor.hpp>
#include <chen/sys/sys.hpp>
#include <sys/event.h>

// -----------------------------------------------------------------------------
// proactor
chen::net::proactor::proactor()
{
    this->_fd = ::kqueue();
    if (this->_fd < 0)
        throw std::system_error(sys::error(), "proactor: failed to create kqueue");
}

chen::net::proactor::~proactor()
{
    ::close(this->_fd);
}

void chen::net::proactor::send(socket_t fd, std::vector<std::uint8_t> &&data)
{
    this->_send[fd].emplace_back(std::move(data));
    this->write(fd);
}

void chen::net::proactor::recv(socket_t fd, std::size_t size)
{
    this->_recv[fd].emplace_back(chunk(size));
    this->read(fd);
}

void chen::net::proactor::loop() throw(std::system_error)
{
    // todo add exit method
    struct kevent event{};

    while (true)
    {
        if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
            throw std::system_error(sys::error(), "proactor: failed to wait event");

        if (event.udata)
            printf("haha: %p\n", event.udata);
//            ptr->onEvent(*this, ::filterToEvent(event.filter, event.flags));
        else
            throw std::system_error(sys::error(), "proactor: event happened but no related socket");
    }
}

// helper
void chen::net::proactor::read(socket_t fd)
{
    struct kevent event = {};
    EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, 0);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(sys::error(), "proactor: failed to add read event");
}

void chen::net::proactor::write(socket_t fd)
{
    struct kevent event = {};
    EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, 0);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(sys::error(), "proactor: failed to add write event");
}

#endif