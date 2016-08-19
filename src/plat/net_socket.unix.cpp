/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/net/net_socket.hpp>
#include <socket/net/net_error.hpp>
#include <chen/sys/sys.hpp>
#include <sys/socket.h>

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
// impl
struct chen::net::socket::impl
{
    int _fd = 0;
};


// -----------------------------------------------------------------------------
// socket
chen::net::socket::socket(Family family, Protocol protocol) : _impl(new impl)
{
    this->create(family, protocol);
}

chen::net::socket::~socket()
{
    this->close();
}

void chen::net::socket::create(Family family, Protocol protocol)
{
    if (this->_impl->_fd)
    {
        // todo notify old socket disconnect

        // close old socket
        this->close();
    }

    this->_family   = family;
    this->_protocol = protocol;

    this->_impl->_fd = ::socket(af(family), type(protocol), 0);
    if (this->_impl->_fd < 0)
        throw error_socket("socket: " + sys::error());
}

// close
void chen::net::socket::shutdown()
{
}

void chen::net::socket::close()
{
}

#endif