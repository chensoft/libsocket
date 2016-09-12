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
chen::socket::socket(socket_t fd) : _fd(fd)
{
//    if (this->_fd < 0)
//        throw socket_error("socket: " + sys::error().message());
}

chen::socket::socket(int family, int type, int protocol)
{
//    this->_fd = ::socket(family, type, protocol);
//    if (this->_fd < 0)
//        throw socket_error("socket: " + sys::error().message());
}

chen::socket::socket(ip::address::Type family, int type, int protocol)
{
//    switch (family)
//    {
//        case ip::address::Type::IPv4:
//            this->_fd = ::socket(AF_INET, type, protocol);
//            break;
//
//        case ip::address::Type::IPv6:
//            this->_fd = ::socket(AF_INET6, type, protocol);
//            break;
//
//        default:
//            break;
//    }
//
//    if (this->_fd < 0)
//        throw socket_error("socket: " + sys::error().message());
}

chen::socket::socket(socket &&o)
{
//    *this = std::move(o);
}

chen::socket& chen::socket::operator=(socket &&o)
{
//    if (this == &o)
//        return *this;
//
//    this->_fd = o._fd;
//    o._fd     = 0;
//
    return *this;
}

chen::socket::~socket()
{
//    this->close();
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

chen::socket_t chen::socket::accept() noexcept
{
    struct sockaddr_storage in{};
    socklen_t len = 0;

    return ::accept(this->_fd, (struct sockaddr*)&in, &len);
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

void chen::socket::close() noexcept
{
    ::close(this->_fd);
    this->_fd = 0;
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

// native
chen::socket_t chen::socket::native() const noexcept
{
    return this->_fd;
}

// event
void chen::socket::onEvent(chen::notifier &n, chen::notifier::Event ev)
{
    // implement in derived class
}

#endif