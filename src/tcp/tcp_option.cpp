/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_option.hpp>

// -----------------------------------------------------------------------------
// option

// nodelay
bool chen::tcp::option::nodelay() const
{
    return bsd::option::intVal(IPPROTO_TCP, TCP_NODELAY) != 0;
}

bool chen::tcp::option::nodelay(bool val)
{
    return !::setsockopt(this->_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}