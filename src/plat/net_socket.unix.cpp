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

// -----------------------------------------------------------------------------
// socket
chen::net::socket::socket(std::nullptr_t)
{
}

chen::net::socket::socket(socket_t fd) : _fd(fd)
{
}

chen::net::socket::socket(int family, int type, int protocol)
{
    this->reset(family, type, protocol);
}

chen::net::socket::socket(address addr, int type, int protocol)
{
    this->reset(addr, type, protocol);
}

chen::net::socket::socket(socket &&o)
{
    *this = std::move(o);
}

chen::net::socket& chen::net::socket::operator=(socket &&o)
{
    if (this == &o)
        return *this;

    this->_fd = o._fd;
    o._fd     = 0;

    return *this;
}

chen::net::socket::~socket()
{
    this->close();
}

// reset
void chen::net::socket::reset(socket_t fd)
{
    if (this->_fd && !this->close())
        throw error_socket("socket: " + sys::error());

    this->_fd = fd;
}

void chen::net::socket::reset(int family, int type, int protocol)
{
    if (this->_fd && !this->close())
        throw error_socket("socket: " + sys::error());

    auto fd = ::socket(family, type, protocol);
    if (fd < 0)
        throw error_socket("socket: " + sys::error());

    this->_fd = fd;
}

void chen::net::socket::reset(address addr, int type, int protocol)
{
    switch (addr.type())
    {
        case address::Type::IPv4:
            return this->reset(AF_INET, type, protocol);

        case address::Type::IPv6:
            return this->reset(AF_INET6, type, protocol);

        default:
            break;
    }
}

// connection
bool chen::net::socket::connect(const endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return !::connect(this->_fd, (struct sockaddr *)&in, len);
}

bool chen::net::socket::bind(const endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return !::bind(this->_fd, (struct sockaddr *)&in, len);
}

bool chen::net::socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

bool chen::net::socket::listen(int backlog) noexcept
{
    return !::listen(this->_fd, backlog);
}

chen::net::socket chen::net::socket::accept()
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    int fd = ::accept(this->_fd, (struct sockaddr*)&in, &len);
    return fd >= 0 ? socket(fd) : nullptr;
}

// data
ssize_t chen::net::socket::send(const void *data, std::size_t size, int flags) noexcept
{
    return ::send(this->_fd, data, size, flags);
}

ssize_t chen::net::socket::send(const std::vector<std::uint8_t> &data, int flags) noexcept
{
    return this->send(data.data(), data.size(), flags);
}

ssize_t chen::net::socket::send(const void *data, std::size_t size, const endpoint &ep, int flags) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return ::sendto(this->_fd, data, size, flags, (struct sockaddr*)&in, len);
}

ssize_t chen::net::socket::send(const std::vector<std::uint8_t> &data, const endpoint &ep, int flags) noexcept
{
    return this->send(data.data(), data.size(), ep, flags);
}

ssize_t chen::net::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, int flags) noexcept
{
    return ::recv(this->_fd, out.data(), size, flags);
}

std::vector<std::uint8_t> chen::net::socket::recv(std::size_t size, int flags) noexcept
{
    std::vector<std::uint8_t> ret;
    this->recv(ret, size, flags);
    return ret;
}

ssize_t chen::net::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, endpoint &ep, int flags) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    auto ret = ::recvfrom(this->_fd, out.data(), size, flags, (struct sockaddr*)&in, &len);
    if (ret >= 0)
        ep = endpoint::toEndpoint(&in);

    return ret;
}

std::vector<std::uint8_t> chen::net::socket::recv(std::size_t size, endpoint &ep, int flags) noexcept
{
    std::vector<std::uint8_t> ret;
    this->recv(ret, size, ep, flags);
    return ret;
}

// error
std::error_code chen::net::socket::error() const noexcept
{
    return std::error_code(errno, std::system_category());
}

// close
bool chen::net::socket::close() noexcept
{
    // treat closed as true
    if (!this->_fd)
        return true;

    // close the socket
    if (::close(this->_fd))
        return false;

    this->_fd = 0;
    return true;
}

bool chen::net::socket::shutdown(Shutdown flag) noexcept
{
    // treat closed as true
    if (!this->_fd)
        return true;

    // shutdown the socket
    switch (flag)
    {
        case Shutdown::Read:
            return !::shutdown(this->_fd, SHUT_RD);

        case Shutdown::Write:
            return !::shutdown(this->_fd, SHUT_WR);

        case Shutdown::Both:
            return !::shutdown(this->_fd, SHUT_RDWR);
    }

    return true;
}

// info
chen::net::endpoint chen::net::socket::local() const noexcept
{
    if (!this->_fd)
        return nullptr;

    struct sockaddr_storage in{};
    socklen_t len = sizeof(in);

    if (::getsockname(this->_fd, (struct sockaddr*)&in, &len) != 0)
        return nullptr;
    else
        return endpoint::toEndpoint(&in);
}

chen::net::endpoint chen::net::socket::remote() const noexcept
{
    if (!this->_fd)
        return nullptr;

    struct sockaddr_storage in{};
    socklen_t len = sizeof(in);

    if (::getpeername(this->_fd, (struct sockaddr*)&in, &len) != 0)
        return nullptr;
    else
        return endpoint::toEndpoint(&in);
}

// native
chen::net::socket_t chen::net::socket::native() const noexcept
{
    return this->_fd;
}

// non-blocking
bool chen::net::socket::nonblocking() const noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return (flag & O_NONBLOCK) == O_NONBLOCK;
}

bool chen::net::socket::nonblocking(bool enable) noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return !::fcntl(this->_fd, F_SETFL, enable ? (flag | O_NONBLOCK) : (flag & ~O_NONBLOCK));
}

// empty
bool chen::net::socket::empty() const noexcept
{
    return !this->_fd;
}

chen::net::socket::operator bool() const noexcept
{
    return !this->empty();
}

#endif