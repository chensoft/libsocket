/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/inet/inet_socket.hpp>

// -----------------------------------------------------------------------------
// socket

// property
chen::inet_endpoint chen::inet_socket::peer() const
{
    return this->_socket.peer();
}

chen::inet_endpoint chen::inet_socket::sock() const
{
    return this->_socket.sock();
}

bool chen::inet_socket::valid() const
{
    return this->_socket.valid();
}

chen::inet_socket::operator bool() const
{
    return this->_socket;
}

std::error_code chen::inet_socket::nonblocking(bool enable)
{
    return this->_socket.nonblocking(enable);
}

// reset
void chen::inet_socket::reset()
{
    this->_socket.reset();
}

void chen::inet_socket::reset(basic_socket &&s)
{
    this->_socket = std::move(s);
}

void chen::inet_socket::reset(ip_address::Type family, int type)
{
    this->_socket.reset(static_cast<int>(family), type);
}