/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_socket.hpp>

// -----------------------------------------------------------------------------
// socket

// error
std::error_code chen::net::socket::error() const
{
    return std::error_code(errno, std::system_category());
}

// info
chen::net::socket::Family chen::net::socket::family() const
{
    return this->_family;
}

chen::net::socket::Protocol chen::net::socket::protocol() const
{
    return this->_protocol;
}