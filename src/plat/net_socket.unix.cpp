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
#include <netdb.h>
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
                auto  in = (struct sockaddr_in*)&ret;

                in->sin_family      = AF_INET;
                in->sin_port        = chen::num::swap(ep.port());
                in->sin_addr.s_addr = chen::num::swap(v4.addr());

                break;
            }

            case chen::net::address::Type::IPv6:
            {
                auto &v6 = ep.addr().v6();
                auto  in = (struct sockaddr_in6*)&ret;

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

    chen::net::endpoint addr(const struct sockaddr *sock)
    {
        switch (sock->sa_family)
        {
            case AF_INET:
            {
                auto in = (struct sockaddr_in*)sock;
                return chen::net::endpoint(chen::net::address(chen::num::swap(in->sin_addr.s_addr)),
                                           chen::num::swap(in->sin_port));
            }

            case AF_INET6:
            {
                auto in = (struct sockaddr_in6*)sock;
                return chen::net::endpoint(chen::net::address(chen::net::version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id)),
                                           chen::num::swap(in->sin6_port));
            }

            default:
                return nullptr;
        }
    }

    chen::net::endpoint addr(const struct sockaddr_storage *sock)
    {
        return addr((const struct sockaddr*)sock);
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
chen::net::socket::socket(std::nullptr_t) : _impl(new impl)
{
}

chen::net::socket::socket(Family family, Protocol protocol) : _impl(new impl)
{
    this->reset(family, protocol);
}

chen::net::socket::socket(socket &&o)
{
    *this = std::move(o);
}

chen::net::socket& chen::net::socket::operator=(socket &&o)
{
    if (this == &o)
        return *this;

    this->_family   = o._family;
    this->_protocol = o._protocol;
    this->_error    = std::move(o._error);
    this->_impl     = std::move(o._impl);

    return *this;
}

chen::net::socket::~socket()
{
    this->close();
}

// connection
bool chen::net::socket::connect(const endpoint &ep) noexcept
{
    auto in = addr(ep);

    if (::connect(this->_impl->_fd, (struct sockaddr *)&in, sizeof(in)))
    {
        this->record();
        return false;
    }

    return true;
}

bool chen::net::socket::connect(const address &addr, std::uint16_t port) noexcept
{
    return this->connect(endpoint(addr, port));
}

bool chen::net::socket::bind(const endpoint &ep) noexcept
{
    auto in = addr(ep);

    if (::bind(this->_impl->_fd, (struct sockaddr *)&in, sizeof(in)))
    {
        this->record();
        return false;
    }

    return true;
}

bool chen::net::socket::bind(const address &addr, std::uint16_t port) noexcept
{
    return this->bind(endpoint(addr, port));
}

bool chen::net::socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

bool chen::net::socket::listen(int backlog) noexcept
{
    if (::listen(this->_impl->_fd, backlog))
    {
        this->record();
        return false;
    }

    return true;
}

chen::net::socket chen::net::socket::accept()
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    int sock = ::accept(this->_impl->_fd, (struct sockaddr*)&in, &len);

    if (sock < 0)
    {
        this->record();
        throw error_socket("socket: " + sys::error());
    }

    socket ret(this->_family, this->_protocol);
    ret._impl->_fd = sock;

    return ret;
}

// data
ssize_t chen::net::socket::send(const void *data, std::size_t size, int flags) noexcept
{
    auto ret = ::send(this->_impl->_fd, data, size, flags);
    if (ret < 0)
        this->record();

    return ret;
}

ssize_t chen::net::socket::send(const std::vector<std::uint8_t> &data, int flags) noexcept
{
    return this->send(data.data(), data.size(), flags);
}

ssize_t chen::net::socket::send(const void *data, std::size_t size, int flags, const endpoint &ep) noexcept
{
    auto in  = addr(ep);
    auto ret = ::sendto(this->_impl->_fd, data, size, flags, (struct sockaddr*)&in, sizeof(in));

    if (ret < 0)
        this->record();

    return ret;
}

ssize_t chen::net::socket::send(const std::vector<std::uint8_t> &data, int flags, const endpoint &ep) noexcept
{
    return this->send(data.data(), data.size(), flags, ep);
}

ssize_t chen::net::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, int flags) noexcept
{
    auto ret = ::recv(this->_impl->_fd, out.data(), size, flags);
    if (ret < 0)
        this->record();

    return ret;
}

std::vector<std::uint8_t> chen::net::socket::recv(std::size_t size, int flags) noexcept
{
    std::vector<std::uint8_t> ret;
    this->recv(ret, size, flags);
    return ret;
}

ssize_t chen::net::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, int flags, endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    auto ret = ::recvfrom(this->_impl->_fd, out.data(), size, flags, (struct sockaddr*)&in, &len);
    if (ret >= 0)
        ep = addr(&in);
    else
        this->record();

    return ret;
}

std::vector<std::uint8_t> chen::net::socket::recv(std::size_t size, int flags, endpoint &ep) noexcept
{
    std::vector<std::uint8_t> ret;
    this->recv(ret, size, flags, ep);
    return ret;
}

// error
std::error_code chen::net::socket::error() const noexcept
{
    return this->_error;
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
    {
        this->record();
        throw error_socket("socket: " + sys::error());
    }

    auto fd = ::socket(af(family), type(protocol), 0);

    if (fd < 0)
    {
        this->record();
        throw error_socket("socket: " + sys::error());
    }

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
    if (::close(this->_impl->_fd))
    {
        this->record();
        return false;
    }

    this->_impl->_fd = 0;
    return true;
}

bool chen::net::socket::shutdown(Shutdown flag) noexcept
{
    // treat closed as true
    if (!this->_impl->_fd)
        return true;

    // shutdown the socket
    int ret = 0;

    switch (flag)
    {
        case Shutdown::Read:
            ret = ::shutdown(this->_impl->_fd, SHUT_RD);
            break;

        case Shutdown::Write:
            ret = ::shutdown(this->_impl->_fd, SHUT_WR);
            break;

        case Shutdown::Both:
            ret = ::shutdown(this->_impl->_fd, SHUT_RDWR);
            break;
    }

    if (ret < 0)
    {
        this->record();
        return false;
    }

    return true;
}

// info
chen::net::endpoint chen::net::socket::local() const noexcept
{
    if (!this->_impl->_fd)
        return nullptr;

    struct sockaddr_storage in{};
    socklen_t len = sizeof(in);

    if (::getsockname(this->_impl->_fd, (struct sockaddr*)&in, &len) != 0)
        return nullptr;
    else
        return addr(&in);
}

chen::net::endpoint chen::net::socket::remote() const noexcept
{
    if (!this->_impl->_fd)
        return nullptr;

    struct sockaddr_storage in{};
    socklen_t len = sizeof(in);

    if (::getpeername(this->_impl->_fd, (struct sockaddr*)&in, &len) != 0)
        return nullptr;
    else
        return addr(&in);
}

// empty
bool chen::net::socket::empty() const noexcept
{
    return !this->_impl->_fd;
}

chen::net::socket::operator bool() const noexcept
{
    return !this->empty();
}

// resolve
std::vector<chen::net::address> chen::net::socket::resolve(const std::string &host) noexcept
{
    struct addrinfo *info = nullptr;
    struct addrinfo hint{};

    hint.ai_family   = AF_UNSPEC;    // IPv4 or IPv6
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), nullptr, &hint, &info))
        return {};

    std::vector<chen::net::address> ret;

    for (struct addrinfo *ptr = info; ptr != nullptr; ptr = ptr->ai_next)
        ret.emplace_back(addr(ptr->ai_addr).addr());

    ::freeaddrinfo(info);

    return ret;
}

// record
void chen::net::socket::record() noexcept
{
    this->_error.assign(errno, std::system_category());
}

#endif