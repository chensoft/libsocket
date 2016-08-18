/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_socket.hpp>
#include <chen/sys/sys.hpp>
#include <map>

#ifndef _WIN32
#include <socket/net/net_error.hpp>
#include <sys/socket.h>

#else
// todo
#endif

// -----------------------------------------------------------------------------
// helper
namespace
{
    int af(chen::net::socket::Family family)
    {
        switch (family)
        {
            case chen::net::socket::Family::IPv4:
                return AF_INET;

            case chen::net::socket::Family::IPv6:
                return AF_INET6;

            case chen::net::socket::Family::Unix:
                return AF_UNIX;
        }
    }

    int type(chen::net::socket::Protocol protocol)
    {
        switch (protocol)
        {
            case chen::net::socket::Protocol::TCP:
                return SOCK_STREAM;

            case chen::net::socket::Protocol::UDP:
                return SOCK_DGRAM;

            case chen::net::socket::Protocol::RAW:
                return SOCK_RAW;
        }
    }
}


// -----------------------------------------------------------------------------
// socket
chen::net::socket::socket(Family family, Protocol protocol) : _family(family), _protocol(protocol)
{
    this->_fd = ::socket(af(family), type(protocol), 0);
    if (this->_fd < 0)
        throw error_socket("socket: " + sys::error());
}

chen::net::socket::~socket()
{
}

// error
std::error_code chen::net::socket::error() const
{
    return std::error_code(errno, std::system_category());
}

// info
chen::net::socket::Family chen::net::socket::family() const
{
    return this->_family;
}

chen::net::socket::Protocol chen::net::socket::protocol() const
{
    return this->_protocol;
}