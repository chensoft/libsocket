/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_socket.hpp>

// -----------------------------------------------------------------------------
// socket
chen::net::socket::socket(bsd::socket &&s) : _handle(std::move(s))
{
}

chen::net::socket::socket(ip::address::Type family, int type) : _handle(static_cast<int>(family), type)
{
}

// property
chen::net::endpoint chen::net::socket::sock() const
{
    return this->_handle.sock();
}

chen::net::endpoint chen::net::socket::peer() const
{
    return this->_handle.peer();
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