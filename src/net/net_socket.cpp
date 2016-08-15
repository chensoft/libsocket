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
#include <sys/socket.h>
#include <socket/net/net_error.hpp>

#else
// todo
#endif

using namespace chen;
using namespace chen::net;

// -----------------------------------------------------------------------------
// helper
namespace
{
    int af(socket::Family family)
    {
        switch (family)
        {
            case socket::Family::IPv4:
                return AF_INET;

            case socket::Family::IPv6:
                return AF_INET6;

            case socket::Family::Unix:
                return AF_UNIX;
        }
    }

    int type(socket::Protocol protocol)
    {
        switch (protocol)
        {
            case socket::Protocol::TCP:
                return SOCK_STREAM;

            case socket::Protocol::UDP:
                return SOCK_DGRAM;

            case socket::Protocol::RAW:
                return SOCK_RAW;
        }
    }
}


// -----------------------------------------------------------------------------
// socket
socket::socket(Family family, Protocol protocol) : _family(family), _protocol(protocol)
{
    this->_fd = ::socket(af(family), type(protocol), 0);
    if (this->_fd < 0)
        throw error_socket("socket: " + sys::error());
}

socket::~socket()
{
}

// info
socket::Family socket::family() const
{
    return this->_family;
}

socket::Protocol socket::protocol() const
{
    return this->_protocol;
}