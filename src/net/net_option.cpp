/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_option.hpp>

// -----------------------------------------------------------------------------
// basic
int chen::net::option::basic::optionInt(socket_t fd, int level, int name)
{
    int val = 0;
    option_t len = sizeof(val);
    return !::getsockopt(fd, level, name, &val, &len) ? val : 0;
}


// -----------------------------------------------------------------------------
// accept_conn
chen::net::option::accept_conn::accept_conn(const socket &sock)
{
    this->listening = basic::optionInt(sock.native(), SOL_SOCKET, SO_ACCEPTCONN) != 0;
}

chen::net::option::accept_conn::accept_conn(bool val) : listening(val)
{
}


// -----------------------------------------------------------------------------
// broadcast
chen::net::option::broadcast::broadcast(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_BROADCAST) != 0;
}

chen::net::option::broadcast::broadcast(bool val) : enable(val)
{
}

bool chen::net::option::broadcast::apply(socket &sock)
{
    // todo sizeof using bool?
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
}