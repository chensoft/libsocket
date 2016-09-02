/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/base/base_socket.hpp>
#include <socket/base/base_error.hpp>
#include <chen/base/num.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// socket
chen::socket::socket(std::nullptr_t)
{
}

chen::socket::socket(socket_t fd) : _fd(fd)
{
}

chen::socket::socket(int family, int type, int protocol)
{
    this->reset(family, type, protocol);
}

chen::socket::socket(ip::address addr, int type, int protocol)
{
    this->reset(addr, type, protocol);
}

chen::socket::socket(socket &&o)
{
    this->reset(std::move(o));
}

chen::socket& chen::socket::operator=(socket &&o)
{
    this->reset(std::move(o));
    return *this;
}

chen::socket::~socket()
{
    this->close();
}

// reset
void chen::socket::reset(socket &&o)
{
    if (this == &o)
        return;

    this->reset(o._fd);
    o._fd = 0;
}

void chen::socket::reset(socket_t fd)
{
    if (this->_fd && !this->close())
        throw socket_error("socket: " + sys::error().message());

    this->_fd = fd;
}

void chen::socket::reset(int family, int type, int protocol)
{
    if (this->_fd && !this->close())
        throw socket_error("socket: " + sys::error().message());

    auto fd = ::socket(family, type, protocol);
    if (fd < 0)
        throw socket_error("socket: " + sys::error().message());

    this->_fd = fd;
}

void chen::socket::reset(ip::address addr, int type, int protocol)
{
    switch (addr.type())
    {
        case ip::address::Type::IPv4:
            return this->reset(AF_INET, type, protocol);

        case ip::address::Type::IPv6:
            return this->reset(AF_INET6, type, protocol);

        default:
            break;
    }
}

// connection
std::error_code chen::socket::connect(const endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return ::connect(this->_fd, (struct sockaddr *)&in, len) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::socket::bind(const endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return ::bind(this->_fd, (struct sockaddr *)&in, len) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

std::error_code chen::socket::listen(int backlog) noexcept
{
    return ::listen(this->_fd, backlog) < 0 ? sys::error() : std::error_code();
}

chen::socket chen::socket::accept() noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    int fd = ::accept(this->_fd, (struct sockaddr*)&in, &len);
    return fd >= 0 ? socket(fd) : nullptr;
}

// data
ssize_t chen::socket::send(const void *data, std::size_t size, int flags) noexcept
{
    return ::send(this->_fd, data, size, flags);
}

ssize_t chen::socket::send(const void *data, std::size_t size, const endpoint &ep, int flags) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return ::sendto(this->_fd, data, size, flags, (struct sockaddr*)&in, len);
}

ssize_t chen::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, int flags) noexcept
{
    return ::recv(this->_fd, out.data(), size, flags);
}

ssize_t chen::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, endpoint &ep, int flags) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    auto ret = ::recvfrom(this->_fd, out.data(), size, flags, (struct sockaddr*)&in, &len);
    if (ret >= 0)
        ep = endpoint::toEndpoint(&in);

    return ret;
}

// close
std::error_code chen::socket::close() noexcept
{
    // treat closed as true
    if (!this->_fd)
        return {};

    // close the socket
    if (::close(this->_fd))
        return sys::error();

    this->_fd = 0;
    return {};
}

std::error_code chen::socket::shutdown(Shutdown flag) noexcept
{
    // treat closed as true
    if (!this->_fd)
        return {};

    // shutdown the socket
    int ret = 0;

    switch (flag)
    {
        case Shutdown::Read:
            ret = ::shutdown(this->_fd, SHUT_RD);
            break;

        case Shutdown::Write:
            ret = ::shutdown(this->_fd, SHUT_WR);
            break;

        case Shutdown::Both:
            ret = ::shutdown(this->_fd, SHUT_RDWR);
            break;
    }

    return ret < 0 ? sys::error() : std::error_code();
}

// info
chen::endpoint chen::socket::local() const noexcept
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

chen::endpoint chen::socket::remote() const noexcept
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

// non-blocking
bool chen::socket::nonblocking() const noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return (flag & O_NONBLOCK) == O_NONBLOCK;
}

bool chen::socket::nonblocking(bool enable) noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return !::fcntl(this->_fd, F_SETFL, enable ? (flag | O_NONBLOCK) : (flag & ~O_NONBLOCK));
}

// empty
bool chen::socket::empty() const noexcept
{
    return !this->_fd;
}

chen::socket::operator bool() const noexcept
{
    return !this->empty();
}

// native
chen::socket_t chen::socket::native() const noexcept
{
    return this->_fd;
}

#endif