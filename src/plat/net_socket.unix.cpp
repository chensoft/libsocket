/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/net/net_socket.hpp>
#include <socket/net/net_error.hpp>
#include <chen/base/num.hpp>
#include <chen/sys/sys.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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

        return 0;
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

        return 0;
    }
}


// -----------------------------------------------------------------------------
// impl
struct chen::net::socket::impl
{
    int _fd = 0;  // socket file descriptor
};


// -----------------------------------------------------------------------------
// socket
chen::net::socket::socket(Family family, Protocol protocol) : _impl(new impl)
{
    this->reset(family, protocol);
}

chen::net::socket::~socket()
{
    this->close();
}

// reset
void chen::net::socket::reset(Family family, Protocol protocol)
{
    if (this->_impl->_fd && !this->close())
        throw error_socket("socket: " + sys::error());

    auto fd = ::socket(af(family), type(protocol), 0);
    if (fd < 0)
        throw error_socket("socket: " + sys::error());

    this->_family    = family;
    this->_protocol  = protocol;
    this->_impl->_fd = fd;
}

// close
bool chen::net::socket::close() noexcept
{
    if (!this->_impl->_fd)
        return true;

    if (!::close(this->_impl->_fd))
    {
        this->_impl->_fd = 0;
        return true;
    }

    return false;
}

bool chen::net::socket::shutdown(Shutdown flag) noexcept
{
    if (!this->_impl->_fd)
        return true;

    if (flag == Shutdown::Read)
        return !::shutdown(this->_impl->_fd, SHUT_RD);
    else if (flag == Shutdown::Write)
        return !::shutdown(this->_impl->_fd, SHUT_WR);
    else if (flag == Shutdown::Both)
        return !::shutdown(this->_impl->_fd, SHUT_RDWR);

    return true;
}

// info
chen::net::endpoint chen::net::socket::local() const noexcept
{
    if (!this->_impl->_fd)
        return nullptr;

    switch (this->_family)
    {
        case socket::Family::IPv4:
        {
            struct sockaddr_in in{};
            socklen_t len = sizeof(in);

            if (::getsockname(this->_impl->_fd, (struct sockaddr*)&in, &len) != 0)
                return nullptr;

            return endpoint(address(num::swap(in.sin_addr.s_addr)), num::swap(in.sin_port));
        }

        case socket::Family::IPv6:
        {
            struct sockaddr_in6 in6{};
            socklen_t len = sizeof(in6);

            if (::getsockname(this->_impl->_fd, (struct sockaddr*)&in6, &len) != 0)
                return nullptr;

            return endpoint(address(version6(version6::array(in6.sin6_addr.s6_addr), 128, in6.sin6_scope_id)),
                            num::swap(in6.sin6_port));
        }

        default:
            return nullptr;
    }
}

chen::net::endpoint chen::net::socket::remote() const noexcept
{
    if (!this->_impl->_fd)
        return nullptr;

    switch (this->_family)
    {
        case socket::Family::IPv4:
        {
            struct sockaddr_in in{};
            socklen_t len = sizeof(in);

            if (::getpeername(this->_impl->_fd, (struct sockaddr*)&in, &len) != 0)
                return nullptr;

            return endpoint(address(num::swap(in.sin_addr.s_addr)), num::swap(in.sin_port));
        }

        case socket::Family::IPv6:
        {
            struct sockaddr_in6 in6{};
            socklen_t len = sizeof(in6);

            if (::getpeername(this->_impl->_fd, (struct sockaddr*)&in6, &len) != 0)
                return nullptr;

            return endpoint(address(version6(version6::array(in6.sin6_addr.s6_addr), 128, in6.sin6_scope_id)),
                            num::swap(in6.sin6_port));
        }

        default:
            return nullptr;
    }
}

#endif