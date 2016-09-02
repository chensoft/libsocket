/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/base_option.hpp>

// -----------------------------------------------------------------------------
// basic
int chen::option::basic::optionInt(socket_t fd, int level, int name)
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
chen::option::acceptconn::acceptconn(const socket &sock)
{
    this->listening = basic::optionInt(sock.native(), SOL_SOCKET, SO_ACCEPTCONN) != 0;
}


// -----------------------------------------------------------------------------
// reuseaddr
chen::option::reuseaddr::reuseaddr(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_REUSEADDR) != 0;
}

chen::option::reuseaddr::reuseaddr(bool val) : enable(val)
{
}

bool chen::option::reuseaddr::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// reuseport
chen::option::reuseport::reuseport(const socket &sock)
{
#ifndef _WIN32
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_REUSEPORT) != 0;
#else
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_REUSEADDR) != 0;
#endif
}

chen::option::reuseport::reuseport(bool val) : enable(val)
{
}

bool chen::option::reuseport::apply(socket &sock)
{
    int val = this->enable;
#ifndef _WIN32
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
#else
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
#endif
}


// -----------------------------------------------------------------------------
// keepalive
chen::option::keepalive::keepalive(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_KEEPALIVE) != 0;
}

chen::option::keepalive::keepalive(bool val) : enable(val)
{
}

bool chen::option::keepalive::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// dontroute
chen::option::dontroute::dontroute(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_DONTROUTE) != 0;
}

chen::option::dontroute::dontroute(bool val) : enable(val)
{
}

bool chen::option::dontroute::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_DONTROUTE, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// broadcast
chen::option::broadcast::broadcast(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), SOL_SOCKET, SO_BROADCAST) != 0;
}

chen::option::broadcast::broadcast(bool val) : enable(val)
{
}

bool chen::option::broadcast::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// linger
chen::option::linger::linger(const socket &sock)
{
    struct ::linger val{};
    option_t len = sizeof(val);

    if (!::getsockopt(sock.native(), SOL_SOCKET, SO_LINGER, &val, &len))
    {
        this->l_onoff  = val.l_onoff;
        this->l_linger = val.l_linger;
    }
}

chen::option::linger::linger(int onoff, int value) : l_onoff(onoff), l_linger(value)
{
}

bool chen::option::linger::apply(socket &sock)
{
    struct ::linger val = {
            .l_onoff  = this->l_onoff,
            .l_linger = this->l_linger
    };

    return !::setsockopt(sock.native(), SOL_SOCKET, SO_LINGER, &val, sizeof(val));
}


// -----------------------------------------------------------------------------
// sndbuf
chen::option::sndbuf::sndbuf(const socket &sock)
{
    this->size = basic::optionInt(sock.native(), SOL_SOCKET, SO_SNDBUF);
}

chen::option::sndbuf::sndbuf(int val) : size(val)
{
}

bool chen::option::sndbuf::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_SNDBUF, &this->size, sizeof(this->size));
}


// -----------------------------------------------------------------------------
// rcvbuf
chen::option::rcvbuf::rcvbuf(const socket &sock)
{
    this->size = basic::optionInt(sock.native(), SOL_SOCKET, SO_RCVBUF);
}

chen::option::rcvbuf::rcvbuf(int val) : size(val)
{
}

bool chen::option::rcvbuf::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_RCVBUF, &this->size, sizeof(this->size));
}


// -----------------------------------------------------------------------------
// sndtimeo
chen::option::sndtimeo::sndtimeo(const socket &sock)
{
    option_t len = sizeof(this->time);
    ::getsockopt(sock.native(), SOL_SOCKET, SO_SNDTIMEO, &this->time, &len);
}

chen::option::sndtimeo::sndtimeo(struct timeval val) : time(val)
{
}

bool chen::option::sndtimeo::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_SNDTIMEO, &this->time, sizeof(this->time));
}


// -----------------------------------------------------------------------------
// rcvtimeo
chen::option::rcvtimeo::rcvtimeo(const socket &sock)
{
    option_t len = sizeof(this->time);
    ::getsockopt(sock.native(), SOL_SOCKET, SO_RCVTIMEO, &this->time, &len);
}

chen::option::rcvtimeo::rcvtimeo(struct timeval val) : time(val)
{
}

bool chen::option::rcvtimeo::apply(socket &sock)
{
    return !::setsockopt(sock.native(), SOL_SOCKET, SO_RCVTIMEO, &this->time, sizeof(this->time));
}


// -----------------------------------------------------------------------------
// error
chen::option::error::error(const socket &sock)
{
    this->code.assign(basic::optionInt(sock.native(), SOL_SOCKET, SO_ERROR), std::system_category());
}


// -----------------------------------------------------------------------------
// type
chen::option::type::type(const socket &sock)
{
    this->val = basic::optionInt(sock.native(), SOL_SOCKET, SO_TYPE);
}


// -----------------------------------------------------------------------------
// nodelay
chen::option::nodelay::nodelay(const socket &sock)
{
    this->enable = basic::optionInt(sock.native(), IPPROTO_TCP, TCP_NODELAY) != 0;
}

chen::option::nodelay::nodelay(bool val) : enable(val)
{
}

bool chen::option::nodelay::apply(socket &sock)
{
    int val = this->enable;
    return !::setsockopt(sock.native(), IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}