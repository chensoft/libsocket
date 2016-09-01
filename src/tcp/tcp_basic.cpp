/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic
chen::net::endpoint chen::tcp::basic::local() const
{
    return this->_socket.local();
}

chen::net::endpoint chen::tcp::basic::remote() const
{
    return this->_socket.remote();
}

bool chen::tcp::basic::nonblocking() const
{
    return this->_socket.nonblocking();
}

bool chen::tcp::basic::nonblocking(bool enable)
{
    return this->_socket.nonblocking(enable);
}

bool chen::tcp::basic::empty() const
{
    return this->_socket.empty();
}

chen::tcp::basic::operator bool() const
{
    return this->_socket;
}

chen::net::socket_t chen::tcp::basic::native() const
{
    return this->_socket.native();
}