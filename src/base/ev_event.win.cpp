/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/inet/inet_address.hpp>
#include <socket/base/ev_event.hpp>
#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// ev_event
chen::ev_event::ev_event(std::function<void()> cb) : _notify(std::move(cb)), _write(AF_INET, SOCK_DGRAM)
{
    // create read socket
    basic_socket tmp(AF_INET, SOCK_DGRAM);

    if (tmp.bind(inet_address("127.0.0.1:0")))
        throw std::system_error(sys::error(), "event: failed to bind on read socket");

    if (tmp.nonblocking(true))
        throw std::system_error(sys::error(), "event: failed to make nonblocking on read socket");

    this->change(tmp.transfer());

    // create write socket
    if (this->_write.nonblocking(true))
        throw std::system_error(sys::error(), "event: failed to make nonblocking on write socket");
}

chen::ev_event::~ev_event()
{
}

void chen::ev_event::set()
{
    ::sockaddr_storage tmp{};
    socklen_t len = sizeof(tmp);

    ::getsockname(this->native(), (::sockaddr*)&tmp, &len);

    if (this->_write.sendto("\n", 1, inet_address((::sockaddr*)&tmp)) != 1)
        throw std::system_error(sys::error(), "event: failed to set event");
}

void chen::ev_event::reset()
{
    char dummy;
    while (::recvfrom(this->native(), &dummy, 1, 0, nullptr, nullptr) >= 0)
        ;
}

// notify
void chen::ev_event::attach(std::function<void()> cb)
{
    this->_notify = std::move(cb);
}

// event
void chen::ev_event::onEvent(int type)
{
    auto loop = this->evLoop();
    auto func = this->_notify;

    if (loop && ((type & Closed) || (this->evFlag() & reactor::FlagOnce)))
        loop->del(this);

    if (func)
        func();
}

#endif