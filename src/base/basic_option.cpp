/**
 * Created by Jian Chen
 * @since  2016.09.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_option.hpp>
#include <socket/base/basic_socket.hpp>

// -----------------------------------------------------------------------------
// basic_option
chen::basic_option::basic_option(basic_socket &s) : _socket(s)
{
}

// debug
bool chen::basic_option::debug() const
{
    return basic_option::get(SOL_SOCKET, SO_DEBUG) != 0;
}

bool chen::basic_option::debug(bool val)
{
    return basic_option::set(SOL_SOCKET, SO_DEBUG, val);
}

// reuseaddr
bool chen::basic_option::reuseaddr() const
{
    return basic_option::get(SOL_SOCKET, SO_REUSEADDR) != 0;
}

bool chen::basic_option::reuseaddr(bool val)
{
    return basic_option::set(SOL_SOCKET, SO_REUSEADDR, val);
}

// reuseport
bool chen::basic_option::reuseport() const
{
#ifdef SO_REUSEPORT
    return basic_option::get(SOL_SOCKET, SO_REUSEPORT) != 0;
#else
    return false;
#endif
}

bool chen::basic_option::reuseport(bool val)
{
#ifdef SO_REUSEPORT
    return basic_option::set(SOL_SOCKET, SO_REUSEPORT, val);
#else
    return false;
#endif
}

// keepalive
bool chen::basic_option::keepalive() const
{
    return basic_option::get(SOL_SOCKET, SO_KEEPALIVE) != 0;
}

bool chen::basic_option::keepalive(bool val)
{
    return basic_option::set(SOL_SOCKET, SO_KEEPALIVE, val);
}

// dontroute
bool chen::basic_option::dontroute() const
{
    return basic_option::get(SOL_SOCKET, SO_DONTROUTE) != 0;
}

bool chen::basic_option::dontroute(bool val)
{
    return basic_option::set(SOL_SOCKET, SO_DONTROUTE, val);
}

// broadcast
bool chen::basic_option::broadcast() const
{
    return basic_option::get(SOL_SOCKET, SO_BROADCAST) != 0;
}

bool chen::basic_option::broadcast(bool val)
{
    return basic_option::set(SOL_SOCKET, SO_BROADCAST, val);
}

// linger
struct ::linger chen::basic_option::linger() const
{
    struct ::linger val{};
    option_t len = sizeof(val);
    return ::getsockopt(this->_socket.handle(), SOL_SOCKET, SO_LINGER, (char*)&val, &len), val;
}

bool chen::basic_option::linger(int onoff, int value)
{
    struct ::linger val{};
    val.l_onoff  = onoff;
    val.l_linger = value;

    return this->linger(val);
}

bool chen::basic_option::linger(const struct ::linger &val)
{
    return !::setsockopt(this->_socket.handle(), SOL_SOCKET, SO_LINGER, (const char*)&val, sizeof(val));
}

// oobinline
bool chen::basic_option::oobinline() const
{
    return basic_option::get(SOL_SOCKET, SO_OOBINLINE) != 0;
}

bool chen::basic_option::oobinline(bool val)
{
    return basic_option::set(SOL_SOCKET, SO_OOBINLINE, val);
}

// sndbuf
int chen::basic_option::sndbuf() const
{
    return basic_option::get(SOL_SOCKET, SO_SNDBUF);
}

bool chen::basic_option::sndbuf(int val)
{
    return basic_option::set(SOL_SOCKET, SO_SNDBUF, val);
}

// rcvbuf
int chen::basic_option::rcvbuf() const
{
    return basic_option::get(SOL_SOCKET, SO_RCVBUF);
}

bool chen::basic_option::rcvbuf(int val)
{
    return basic_option::set(SOL_SOCKET, SO_RCVBUF, val);
}

// sndlowat
int chen::basic_option::sndlowat() const
{
    return basic_option::get(SOL_SOCKET, SO_SNDLOWAT);
}

bool chen::basic_option::sndlowat(int val)
{
    return basic_option::set(SOL_SOCKET, SO_SNDLOWAT, val);
}

// rcvlowat
int chen::basic_option::rcvlowat() const
{
    return basic_option::get(SOL_SOCKET, SO_RCVLOWAT);
}

bool chen::basic_option::rcvlowat(int val)
{
    return basic_option::set(SOL_SOCKET, SO_RCVLOWAT, val);
}

// sndtimeo
struct ::timeval chen::basic_option::sndtimeo() const
{
    ::timeval val{};
    option_t len = sizeof(val);
    return ::getsockopt(this->_socket.handle(), SOL_SOCKET, SO_SNDTIMEO, (char*)&val, &len), val;
}

bool chen::basic_option::sndtimeo(int sec, int usec)
{
    ::timeval val{};
    val.tv_sec  = sec;
    val.tv_usec = usec;

    return this->sndtimeo(val);
}

bool chen::basic_option::sndtimeo(const struct ::timeval &time)
{
    return !::setsockopt(this->_socket.handle(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&time, sizeof(time));
}

// rcvtimeo
struct ::timeval chen::basic_option::rcvtimeo() const
{
    ::timeval val{};
    option_t len = sizeof(val);
    return ::getsockopt(this->_socket.handle(), SOL_SOCKET, SO_RCVTIMEO, (char*)&val, &len), val;
}

bool chen::basic_option::rcvtimeo(int sec, int usec)
{
    ::timeval val{};
    val.tv_sec  = sec;
    val.tv_usec = usec;

    return this->rcvtimeo(val);
}

bool chen::basic_option::rcvtimeo(const struct ::timeval &time)
{
    return !::setsockopt(this->_socket.handle(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
}

// error
std::error_code chen::basic_option::error() const
{
    return std::error_code(basic_option::get(SOL_SOCKET, SO_ERROR), std::generic_category());
}

// type
int chen::basic_option::type() const
{
    return basic_option::get(SOL_SOCKET, SO_TYPE);
}

// helper
int chen::basic_option::get(int level, int name) const
{
    int val = 0;
    option_t len = sizeof(val);
    return ::getsockopt(this->_socket.handle(), level, name, (char*)&val, &len), val;
}

bool chen::basic_option::set(int level, int name, int val)
{
    return !::setsockopt(this->_socket.handle(), level, name, (const char*)&val, sizeof(val));
}