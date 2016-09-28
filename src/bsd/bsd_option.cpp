/**
 * Created by Jian Chen
 * @since  2016.09.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/bsd_option.hpp>

// -----------------------------------------------------------------------------
// option
chen::bsd::option::option(socket_t fd) : _fd(fd)
{
}

// debug
bool chen::bsd::option::debug() const
{
    return option::get(SOL_SOCKET, SO_DEBUG) != 0;
}

bool chen::bsd::option::debug(bool val)
{
    return option::set(SOL_SOCKET, SO_DEBUG, val);
}

// acceptconn
bool chen::bsd::option::acceptconn() const
{
    return option::get(SOL_SOCKET, SO_ACCEPTCONN) != 0;
}

// reuseaddr
bool chen::bsd::option::reuseaddr() const
{
    return option::get(SOL_SOCKET, SO_REUSEADDR) != 0;
}

bool chen::bsd::option::reuseaddr(bool val)
{
    return option::set(SOL_SOCKET, SO_REUSEADDR, val);
}

// reuseport
bool chen::bsd::option::reuseport() const
{
#ifdef SO_REUSEPORT
    return option::get(SOL_SOCKET, SO_REUSEPORT) != 0;
#else
    return false;
#endif
}

bool chen::bsd::option::reuseport(bool val)
{
#ifdef SO_REUSEPORT
    return option::set(SOL_SOCKET, SO_REUSEPORT, val);
#else
    return false;
#endif
}

// keepalive
bool chen::bsd::option::keepalive() const
{
    return option::get(SOL_SOCKET, SO_KEEPALIVE) != 0;
}

bool chen::bsd::option::keepalive(bool val)
{
    return option::set(SOL_SOCKET, SO_KEEPALIVE, val);
}

// dontroute
bool chen::bsd::option::dontroute() const
{
    return option::get(SOL_SOCKET, SO_DONTROUTE) != 0;
}

bool chen::bsd::option::dontroute(bool val)
{
    return option::set(SOL_SOCKET, SO_DONTROUTE, val);
}

// broadcast
bool chen::bsd::option::broadcast() const
{
    return option::get(SOL_SOCKET, SO_BROADCAST) != 0;
}

bool chen::bsd::option::broadcast(bool val)
{
    return option::set(SOL_SOCKET, SO_BROADCAST, val);
}

// linger
struct ::linger chen::bsd::option::linger() const
{
    struct ::linger val{};
    option_t len = sizeof(val);
    return ::getsockopt(this->_fd, SOL_SOCKET, SO_LINGER, &val, &len), val;
}

bool chen::bsd::option::linger(int onoff, int value)
{
    struct ::linger val = {
            .l_onoff  = onoff,
            .l_linger = value
    };
    return this->linger(val);
}

bool chen::bsd::option::linger(const struct ::linger &val)
{
    return !::setsockopt(this->_fd, SOL_SOCKET, SO_LINGER, &val, sizeof(val));
}

// oobinline
bool chen::bsd::option::oobinline() const
{
    return option::get(SOL_SOCKET, SO_OOBINLINE) != 0;
}

bool chen::bsd::option::oobinline(bool val)
{
    return option::set(SOL_SOCKET, SO_OOBINLINE, val);
}

// sndbuf
int chen::bsd::option::sndbuf() const
{
    return option::get(SOL_SOCKET, SO_SNDBUF);
}

bool chen::bsd::option::sndbuf(int val)
{
    return option::set(SOL_SOCKET, SO_SNDBUF, val);
}

// rcvbuf
int chen::bsd::option::rcvbuf() const
{
    return option::get(SOL_SOCKET, SO_RCVBUF);
}

bool chen::bsd::option::rcvbuf(int val)
{
    return option::set(SOL_SOCKET, SO_RCVBUF, val);
}

// sndlowat
int chen::bsd::option::sndlowat() const
{
    return option::get(SOL_SOCKET, SO_SNDLOWAT);
}

bool chen::bsd::option::sndlowat(int val)
{
    return option::set(SOL_SOCKET, SO_SNDLOWAT, val);
}

// rcvlowat
int chen::bsd::option::rcvlowat() const
{
    return option::get(SOL_SOCKET, SO_RCVLOWAT);
}

bool chen::bsd::option::rcvlowat(int val)
{
    return option::set(SOL_SOCKET, SO_RCVLOWAT, val);
}

// sndtimeo
struct ::timeval chen::bsd::option::sndtimeo() const
{
    struct ::timeval val{};
    option_t len = sizeof(val);
    return ::getsockopt(this->_fd, SOL_SOCKET, SO_SNDTIMEO, &val, &len), val;
}

bool chen::bsd::option::sndtimeo(int sec, int usec)
{
    struct ::timeval val = {
            .tv_sec  = sec,
            .tv_usec = usec
    };
    return this->sndtimeo(val);
}

bool chen::bsd::option::sndtimeo(const struct ::timeval &time)
{
    return !::setsockopt(this->_fd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
}

// rcvtimeo
struct ::timeval chen::bsd::option::rcvtimeo() const
{
    struct ::timeval val{};
    option_t len = sizeof(val);
    return ::getsockopt(this->_fd, SOL_SOCKET, SO_RCVTIMEO, &val, &len), val;
}

bool chen::bsd::option::rcvtimeo(int sec, int usec)
{
    struct ::timeval val = {
            .tv_sec  = sec,
            .tv_usec = usec
    };
    return this->rcvtimeo(val);
}

bool chen::bsd::option::rcvtimeo(const struct ::timeval &time)
{
    return !::setsockopt(this->_fd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
}

// error
std::error_code chen::bsd::option::error() const
{
    return std::error_code(option::get(SOL_SOCKET, SO_ERROR), std::system_category());
}

// type
int chen::bsd::option::type() const
{
    return option::get(SOL_SOCKET, SO_TYPE);
}

// helper
int chen::bsd::option::get(int level, int name) const
{
    int val = 0;
    option_t len = sizeof(val);
    return ::getsockopt(this->_fd, level, name, &val, &len), val;
}

bool chen::bsd::option::set(int level, int name, int val)
{
    return !::setsockopt(this->_fd, level, name, &val, sizeof(val));
}