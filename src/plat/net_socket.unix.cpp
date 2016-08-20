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
#include <cstring>

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

    struct sockaddr_storage addr(const chen::net::endpoint &ep)
    {
        struct sockaddr_storage ret{};

        switch (ep.addr().type())
        {
            case chen::net::address::Type::IPv4:
            {
                auto &v4 = ep.addr().v4();

                struct sockaddr_in *in = (struct sockaddr_in*)&ret;

                in->sin_family      = AF_INET;
                in->sin_port        = chen::num::swap(ep.port());
                in->sin_addr.s_addr = chen::num::swap(v4.addr());

                break;
            }

            case chen::net::address::Type::IPv6:
            {
                auto &v6 = ep.addr().v6();

                struct sockaddr_in6 *in = (struct sockaddr_in6*)&ret;

                in->sin6_family   = AF_INET6;
                in->sin6_port     = chen::num::swap(ep.port());
                in->sin6_scope_id = v6.scope();

                ::memcpy(in->sin6_addr.s6_addr, v6.addr().data(), 16);

                break;
            }

            default:
                break;
        }

        return ret;
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

// connection
bool chen::net::socket::connect(const endpoint &ep) noexcept
{
    auto in = addr(ep);
    return !::connect(this->_impl->_fd, (struct sockaddr *)&in, sizeof(in));
}

bool chen::net::socket::bind(const endpoint &ep) noexcept
{
    auto in = addr(ep);
    return !::bind(this->_impl->_fd, (struct sockaddr *)&in, sizeof(in));
}

bool chen::net::socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

bool chen::net::socket::listen(int backlog) noexcept
{
    return !::listen(this->_impl->_fd, backlog);
}

chen::net::endpoint chen::net::socket::accept() noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    if (::accept(this->_impl->_fd, (struct sockaddr*)&in, &len))
        return nullptr;

    switch (in.ss_family)
    {
        case AF_INET:
        {
            auto tmp = (struct sockaddr_in*)&in;
            return endpoint(address(num::swap(tmp->sin_addr.s_addr)), num::swap(tmp->sin_port));
        }

        case AF_INET6:
        {
            auto tmp = (struct sockaddr_in6*)&in;
            return endpoint(address(version6(version6::array(tmp->sin6_addr.s6_addr), 128, tmp->sin6_scope_id)),
                            num::swap(tmp->sin6_port));
        }

        default:
            return nullptr;
    }
}

// error
std::error_code chen::net::socket::error() const noexcept
{
    // todo how about SO_ERROR?
    return std::error_code(errno, std::system_category());
}

// info
chen::net::socket::Family chen::net::socket::family() const noexcept
{
    return this->_family;
}

chen::net::socket::Protocol chen::net::socket::protocol() const noexcept
{
    return this->_protocol;
}

// reset
void chen::net::socket::reset()
{
    this->reset(this->_family, this->_protocol);
}

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
    // treat closed as true
    if (!this->_impl->_fd)
        return true;

    // close the socket
    if (!::close(this->_impl->_fd))
    {
        this->_impl->_fd = 0;
        return true;
    }

    return false;
}

bool chen::net::socket::shutdown(Shutdown flag) noexcept
{
    // treat closed as true
    if (!this->_impl->_fd)
        return true;

    // shutdown the socket
    switch (flag)
    {
        case Shutdown::Read:
            return !::shutdown(this->_impl->_fd, SHUT_RD);

        case Shutdown::Write:
            return !::shutdown(this->_impl->_fd, SHUT_WR);

        case Shutdown::Both:
            return !::shutdown(this->_impl->_fd, SHUT_RDWR);
    }

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
            struct sockaddr_in6 in{};
            socklen_t len = sizeof(in);

            if (::getsockname(this->_impl->_fd, (struct sockaddr*)&in, &len) != 0)
                return nullptr;

            return endpoint(address(version6(version6::array(in.sin6_addr.s6_addr), 128, in.sin6_scope_id)),
                            num::swap(in.sin6_port));
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
            struct sockaddr_in6 in{};
            socklen_t len = sizeof(in);

            if (::getpeername(this->_impl->_fd, (struct sockaddr*)&in, &len) != 0)
                return nullptr;

            return endpoint(address(version6(version6::array(in.sin6_addr.s6_addr), 128, in.sin6_scope_id)),
                            num::swap(in.sin6_port));
        }

        default:
            return nullptr;
    }
}

#endif