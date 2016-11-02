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
    return this->_handle.peer();
}

chen::net::endpoint chen::net::socket::sock() const
{
    return this->_handle.sock();
}

std::error_code chen::net::socket::nonblocking(bool enable)
{
    return this->_handle.nonblocking(enable);
}

bool chen::net::socket::valid() const
{
    return this->_handle.valid();
}

chen::net::socket::operator bool() const
{
    return this->_handle;
}

// reset
void chen::net::socket::reset()
{
    this->_handle.reset();
}

void chen::net::socket::reset(bsd::socket &&s)
{
    this->_handle = std::move(s);
}

void chen::net::socket::reset(ip::address::Type family, int type)
{
    this->_handle.reset(static_cast<int>(family), type);
}