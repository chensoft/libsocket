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
void chen::net::socket::reset()
{
    // todo
}

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

// connection
bool chen::net::socket::connect(const endpoint &ep) noexcept
{
    return this->connect(ep.addr(), ep.port());
}

bool chen::net::socket::connect(const address &addr, std::uint16_t port) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(addr, port, in, len);
    return !::connect(this->_fd, (struct sockaddr *)&in, len);
}

bool chen::net::socket::bind(const endpoint &ep) noexcept
{
    return this->bind(ep.addr(), ep.port());
}

bool chen::net::socket::bind(const address &addr, std::uint16_t port) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(addr, port, in, len);
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

ssize_t chen::net::socket::send(const void *data, std::size_t size, int flags, const endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    endpoint::toAddress(ep, in, len);
    return ::sendto(this->_fd, data, size, flags, (struct sockaddr*)&in, len);
}

ssize_t chen::net::socket::send(const std::vector<std::uint8_t> &data, int flags, const endpoint &ep) noexcept
{
    return this->send(data.data(), data.size(), flags, ep);
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

ssize_t chen::net::socket::recv(std::vector<std::uint8_t> &out, std::size_t size, int flags, endpoint &ep) noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    auto ret = ::recvfrom(this->_fd, out.data(), size, flags, (struct sockaddr*)&in, &len);
    if (ret >= 0)
        ep = endpoint::toEndpoint(&in);

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
    if (!this->_fd)
        return std::error_code(0, std::system_category());

    int val = 0;
    socklen_t len = 0;

    if (!::getsockopt(this->_fd, SOL_SOCKET, SO_ERROR, &val, &len))
        return std::error_code(val, std::system_category());
    else
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

// empty
bool chen::net::socket::empty() const noexcept
{
    return !this->_fd;
}

chen::net::socket::operator bool() const noexcept
{
    return !this->empty();
}

// resolve
std::vector<chen::net::address> chen::net::socket::resolve(const std::string &host) noexcept
{
    return socket::resolve(host, AF_UNSPEC);  // IPv4 or IPv6
}

std::vector<chen::net::address> chen::net::socket::resolve(const std::string &host, int family) noexcept
{
    struct addrinfo *info = nullptr;
    struct addrinfo hint{};

    hint.ai_family   = family;
    hint.ai_socktype = SOCK_STREAM;  // prevent return same addresses

    if (::getaddrinfo(host.c_str(), nullptr, &hint, &info))
        return {};

    std::vector<chen::net::address> ret;

    for (struct addrinfo *ptr = info; ptr != nullptr; ptr = ptr->ai_next)
        ret.emplace_back(endpoint::toEndpoint(ptr->ai_addr).addr());

    ::freeaddrinfo(info);

    return ret;
}

#endif