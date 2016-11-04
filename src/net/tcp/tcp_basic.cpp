/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic

// option
chen::tcp::option chen::tcp::basic::option()
{
    return tcp::option(this->_socket);
}

// reset
void chen::tcp::basic::reset(ip::address::Type family)
{
    inet_socket::reset(family, SOCK_STREAM);
}