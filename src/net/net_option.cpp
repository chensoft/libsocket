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

    if (!::getsockopt(fd, level, name, &val, &len))
        return val;
    else
        return 0;
}


// -----------------------------------------------------------------------------
// acceptconn
chen::net::option::acceptconn::acceptconn(const socket &sock)
{
    this->listening = basic::optionInt(sock.native(), SOL_SOCKET, SO_ACCEPTCONN) != 0;
}


// -----------------------------------------------------------------------------
// reuseaddr
chen::net::option::reuseaddr::reuseaddr(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_REUSEADDR) != 0;
}

chen::net::option::reuseaddr::reuseaddr(bool val) : enable(val)
{
}

bool chen::net::option::reuseaddr::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// keepalive
chen::net::option::keepalive::keepalive(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_KEEPALIVE) != 0;
}

chen::net::option::keepalive::keepalive(bool val) : enable(val)
{
}

bool chen::net::option::keepalive::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// dontroute
chen::net::option::dontroute::dontroute(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_DONTROUTE) != 0;
}

chen::net::option::dontroute::dontroute(bool val) : enable(val)
{
}

bool chen::net::option::dontroute::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_DONTROUTE, &val, sizeof(val));
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
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// linger
chen::net::option::linger::linger(const socket &sock)
{
    struct ::linger val{};
    option_t len = sizeof(val);

    if (!::getsockopt(sock.native(), SOL_SOCKET, SO_LINGER, &val, &len))
    {
        this->l_onoff  = val.l_onoff;
        this->l_linger = val.l_linger;
    }
}

chen::net::option::linger::linger(int onoff, int value) : l_onoff(onoff), l_linger(value)
{
}

bool chen::net::option::linger::apply(socket &sock)
{
    struct ::linger val = {
            .l_onoff  = this->l_onoff,
            .l_linger = this->l_linger
    };

    return !::setsockopt(sock.native(), SOL_SOCKET, SO_LINGER, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// sndbuf
chen::net::option::sndbuf::sndbuf(const socket &sock)
{
    this->size = basic::optionInt(sock.native(), SOL_SOCKET, SO_SNDBUF);
}

chen::net::option::sndbuf::sndbuf(int val) : size(val)
{
}

bool chen::net::option::sndbuf::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_SNDBUF, &this->size, sizeof(this->size));
}


// -----------------------------------------------------------------------------
// rcvbuf
chen::net::option::rcvbuf::rcvbuf(const socket &sock)
{
    this->size = basic::optionInt(sock.native(), SOL_SOCKET, SO_RCVBUF);
}

chen::net::option::rcvbuf::rcvbuf(int val) : size(val)
{
}

bool chen::net::option::rcvbuf::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_RCVBUF, &this->size, sizeof(this->size));
}


// -----------------------------------------------------------------------------
// sndtimeo
chen::net::option::sndtimeo::sndtimeo(const socket &sock)
{
    option_t len = sizeof(this->time);
    ::getsockopt(sock.native(), SOL_SOCKET, SO_SNDTIMEO, &this->time, &len);
}

chen::net::option::sndtimeo::sndtimeo(struct timeval val) : time(val)
{
}

bool chen::net::option::sndtimeo::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_SNDTIMEO, &this->time, sizeof(this->time));
}


// -----------------------------------------------------------------------------
// rcvtimeo
chen::net::option::rcvtimeo::rcvtimeo(const socket &sock)
{
    option_t len = sizeof(this->time);
    ::getsockopt(sock.native(), SOL_SOCKET, SO_RCVTIMEO, &this->time, &len);
}

chen::net::option::rcvtimeo::rcvtimeo(struct timeval val) : time(val)
{
}

bool chen::net::option::rcvtimeo::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_RCVTIMEO, &this->time, sizeof(this->time));
}


// -----------------------------------------------------------------------------
// error
chen::net::option::error::error(const socket &sock)
{
    this->code.assign(basic::optionInt(sock.native(), SOL_SOCKET, SO_ERROR), std::system_category());
}


// -----------------------------------------------------------------------------
// type
chen::net::option::type::type(const socket &sock)
{
    this->val = basic::optionInt(sock.native(), SOL_SOCKET, SO_TYPE);
}