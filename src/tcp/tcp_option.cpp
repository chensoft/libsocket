/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_option.hpp>

// -----------------------------------------------------------------------------
// nodelay
chen::tcp::option::nodelay::nodelay(const socket &sock)
{
    this->enable = chen::option::basic::optionInt(sock.native(), IPPROTO_TCP, TCP_NODELAY) != 0;
}

chen::tcp::option::nodelay::nodelay(bool val) : enable(val)
{
}

bool chen::tcp::option::nodelay::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}