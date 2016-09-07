/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic
chen::tcp::basic::basic(socket_t fd) : socket(fd)
{
}

chen::tcp::basic::basic(int family, int type, int protocol) : socket(family, type, protocol)
{
}

chen::tcp::basic::basic(ip::address::Type family, int type, int protocol) : socket(family, type, protocol)
{
}