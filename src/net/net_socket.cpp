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
std::error_code chen::net::socket::error() const noexcept
{
    // todo how about SO_ERROR?
    return std::error_code(errno, std::system_category());
}

// info
chen::net::socket::Family chen::net::socket::family() const noexcept
{
    return this->_family;
}

chen::net::socket::Protocol chen::net::socket::protocol() const noexcept
{
    return this->_protocol;
}

// reset
void chen::net::socket::reset()
{
    this->reset(this->_family, this->_protocol);
}