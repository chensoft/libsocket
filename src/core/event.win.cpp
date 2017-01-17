/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/inet/inet_address.hpp>
#include <socket/core/event.hpp>
#include <socket/core/ioctl.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// event
chen::event::event()
{
    // create read socket
    basic_socket tmp(AF_INET, SOCK_DGRAM);

    if (tmp.bind(inet_address("127.0.0.1:0")))
        throw std::system_error(sys::error(), "event: failed to bind on read socket");

    if (tmp.nonblocking(true))
        throw std::system_error(sys::error(), "event: failed to make nonblocking on read socket");

    this->_handle.change(tmp.handle().transfer());

    // create write socket
    this->_write.reset(AF_INET, SOCK_DGRAM, 0);

    if (this->_write.nonblocking(true))
        throw std::system_error(sys::error(), "event: failed to make nonblocking on write socket");
}

chen::event::~event()
{
}

void chen::event::set()
{
    basic_address a;
    ::getsockname(this->_handle, (::sockaddr*)&a.addr, &a.size);

    if (this->_write.sendto("\n", 1, a) != 1)
        throw std::system_error(sys::error(), "event: failed to set event");
}

void chen::event::reset()
{
    char dummy;
    while (::recvfrom(this->_handle, &dummy, 1, 0, nullptr, nullptr) >= 0)
        ;
}

chen::basic_handle& chen::event::handle()
{
    return this->_handle;
}

#endif