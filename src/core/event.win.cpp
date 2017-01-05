/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/inet/inet_address.hpp>
#include <socket/core/event.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// event
chen::event::event()
{
    this->_socket.reset(AF_INET, SOCK_DGRAM);

    if (this->_socket.bind(inet_address("127.0.0.1:0")))
        throw std::system_error(sys::error(), "event: failed to bind on socket");

    if (this->_socket.nonblocking(true))
        throw std::system_error(sys::error(), "event: failed to make nonblocking on socket");
}

chen::event::~event()
{
}

void chen::event::set()
{
    basic_socket s(AF_INET, SOCK_DGRAM);

    // since it's a new socket, data should be written to buffer immediately
    if (s.nonblocking(true) || (s.sendto("\n", 1, this->_socket.sock()) != 1))
        throw std::system_error(sys::error(), "event: failed to set event");
}

void chen::event::reset()
{
    char dummy;
    basic_address addr;

    while (this->_socket.recvfrom(&dummy, 1, addr) >= 0)
        ;
}

chen::handle_t chen::event::native() const
{
    return this->_socket.native();
}

#endif