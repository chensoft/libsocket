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

chen::net::socket::~socket()
{
}

chen::bsd::socket& chen::net::socket::handle()
{
    return this->_handle;
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

int chen::net::socket::family() const
{
    return this->_handle.family();
}

int chen::net::socket::type() const
{
    return this->_handle.type();
}

int chen::net::socket::protocol() const
{
    return this->_handle.protocol();
}