/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/inet/inet_address.hpp>
#include <socket/base/basic_socket.hpp>
#include <socket/core/event.hpp>
#include <socket/core/ioctl.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// event
chen::event::event()
{
    basic_socket tmp(AF_INET, SOCK_DGRAM);

    if (tmp.bind(inet_address("127.0.0.1:0")))
        throw std::system_error(sys::error(), "event: failed to bind on socket");

    if (tmp.nonblocking(true))
        throw std::system_error(sys::error(), "event: failed to make nonblocking on socket");

    this->_handle.change(tmp.handle().transfer());
}

chen::event::~event()
{
}

void chen::event::set()
{
    basic_socket s(AF_INET, SOCK_DGRAM);
    basic_address a;

    ::getsockname(this->_handle, (::sockaddr*)&a.addr, &a.size);

    // since it's a new socket, data should be written to buffer immediately
    if (s.nonblocking(true) || (s.sendto("\n", 1, a) != 1))
        throw std::system_error(sys::error(), "event: failed to set event");
}

void chen::event::reset()
{
    char dummy;
    basic_address addr;

    while (::recvfrom(this->_handle, &dummy, 1, 0, (sockaddr*)&addr.addr, &addr.size) >= 0)
        ;
}

chen::basic_handle& chen::event::handle()
{
    return this->_handle;
}

#endif