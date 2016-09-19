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
chen::net::proactor::proactor() throw(std::system_error)
{
    if ((this->_fd = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "proactor: failed to create kqueue");
}

chen::net::proactor::~proactor()
{
    ::close(this->_fd);
}

void chen::net::proactor::send(net::socket *ptr, std::vector<std::uint8_t> &&data)
{
    this->_send[ptr].emplace_back(std::move(data));
    this->write(ptr);
}

void chen::net::proactor::recv(net::socket *ptr, std::size_t size)
{
    this->_recv[ptr].emplace_back(chunk(size));
    this->read(ptr);
}

void chen::net::proactor::loop() throw(std::system_error)
{
    // todo add exit method
    struct kevent event{};

    while (true)
    {
        if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
            throw std::system_error(sys::error(), "proactor: failed to wait event");

        auto ptr = static_cast<net::socket*>(event.udata);

        if (ptr)
        {
            if (event.flags & EV_EOF)
            {
                // todo clear cache first?
                ptr->onEventEOF();
            }
            else if (event.filter & EVFILT_WRITE)
            {
                // simulate proactor, send data to remote and notify socket
                auto &list = this->_send[ptr];
                if (list.empty())
                    continue;

                auto &chunk = list.front();
                auto   size = ptr->handle().send(chunk.data(), chunk.size());

//                ptr->onEventSend();
            }
            else if (event.filter & EVFILT_READ)
            {
//                ptr->onEventRecv();
            }
            else
            {
                throw std::system_error(sys::error(), "proactor: event happened but flags and filter are unknown");
            }
        }
        else
        {
            throw std::system_error(sys::error(), "proactor: event happened but no related socket");
        }
    }
}

// helper
void chen::net::proactor::write(net::socket *ptr) throw(std::system_error)
{
    struct kevent event{};
    EV_SET(&event, ptr->handle().native(), EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, ptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(sys::error(), "proactor: failed to add write event");
}

void chen::net::proactor::read(net::socket *ptr) throw(std::system_error)
{
    struct kevent event{};
    EV_SET(&event, ptr->handle().native(), EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, ptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(sys::error(), "proactor: failed to add read event");
}

#endif