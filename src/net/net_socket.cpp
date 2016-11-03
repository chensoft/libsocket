/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_socket.hpp>

// -----------------------------------------------------------------------------
// socket

// property
chen::net::endpoint chen::net::socket::peer() const
{
    return this->_socket.peer();
}

chen::net::endpoint chen::net::socket::sock() const
{
    return this->_socket.sock();
}

bool chen::net::socket::valid() const
{
    return this->_socket.valid();
}

chen::net::socket::operator bool() const
{
    return this->_socket;
}

std::error_code chen::net::socket::nonblocking(bool enable)
{
    return this->_socket.nonblocking(enable);
}

// reset
void chen::net::socket::reset()
{
    this->_socket.reset();
}

void chen::net::socket::reset(bsd::socket &&s)
{
    this->_socket = std::move(s);
}

void chen::net::socket::reset(ip::address::Type family, int type)
{
    this->_socket.reset(static_cast<int>(family), type);
}