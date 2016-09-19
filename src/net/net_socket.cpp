/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_socket.hpp>

// -----------------------------------------------------------------------------
// socket
chen::net::socket::socket(socket_t fd) : _handle(fd)
{
}

chen::net::socket::socket(ip::address::Type family, int type) : _handle(static_cast<int>(family), type, 0)
{
}

chen::bsd::socket& chen::net::socket::handle()
{
    return this->_handle;
}

// property
chen::net::endpoint chen::net::socket::local() const
{
    net::endpoint ep;
    this->_handle.local(ep);
    return ep;
}

chen::net::endpoint chen::net::socket::remote() const
{
    net::endpoint ep;
    this->_handle.remote(ep);
    return ep;
}

bool chen::net::socket::nonblocking() const
{
    return this->_handle.nonblocking();
}

bool chen::net::socket::nonblocking(bool enable)
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

chen::socket_t chen::net::socket::native() const
{
    return this->_handle.native();
}