/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic
chen::tcp::option chen::tcp::basic::option()
{
    return tcp::option(this->_handle);
}

// reset
void chen::tcp::basic::reset(ip::address::Type family)
{
    net::socket::reset(family, SOCK_STREAM);
}