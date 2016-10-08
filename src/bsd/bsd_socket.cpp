/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/bsd_socket.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// socket
const chen::socket_t chen::bsd::socket::invalid_handle = (chen::socket_t)-1;

chen::bsd::socket::socket(socket_t fd) noexcept : _fd(fd)
{
}

chen::bsd::socket::socket(int domain, int type, int protocol) : _domain(domain), _type(type), _protocol(protocol)
{
    this->reset();
}

chen::bsd::socket::~socket() noexcept
{
    this->close();
}

// connection
std::error_code chen::bsd::socket::connect(const bsd::endpoint &ep) noexcept
{
    return ::connect(this->_fd, (struct ::sockaddr*)&ep.addr, ep.size) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::bsd::socket::bind(const bsd::endpoint &ep) noexcept
{
    return ::bind(this->_fd, (struct ::sockaddr*)&ep.addr, ep.size) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::bsd::socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

std::error_code chen::bsd::socket::listen(int backlog) noexcept
{
    return ::listen(this->_fd, backlog) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::bsd::socket::accept(socket_t &fd) noexcept
{
    if ((fd = ::accept(this->_fd, nullptr, nullptr)) < 0)
        return sys::error();

    return {};
}

std::error_code chen::bsd::socket::accept(socket_t &fd, bsd::endpoint &ep) noexcept
{
    if ((fd = ::accept(this->_fd, (struct ::sockaddr*)&ep.addr, &ep.size)) < 0)
        return sys::error();

    return {};
}

// transmission
ssize_t chen::bsd::socket::send(const void *data, std::size_t size, int flags) noexcept
{
    return ::send(this->_fd, data, size, flags);
}

ssize_t chen::bsd::socket::send(const void *data, std::size_t size, const bsd::endpoint &ep, int flags) noexcept
{
    return ::sendto(this->_fd, data, size, flags, (struct ::sockaddr*)&ep.addr, ep.size);
}

ssize_t chen::bsd::socket::recv(void *data, std::size_t size, int flags) noexcept
{
    return ::recv(this->_fd, data, size, flags);
}

ssize_t chen::bsd::socket::recv(void *data, std::size_t size, bsd::endpoint &ep, int flags) noexcept
{
    return ::recvfrom(this->_fd, data, size, flags, (struct ::sockaddr*)&ep.addr, &ep.size);
}

// cleanup
void chen::bsd::socket::shutdownBoth() noexcept
{
    ::shutdown(this->_fd, SHUT_RDWR);
}

void chen::bsd::socket::shutdownRead() noexcept
{
    ::shutdown(this->_fd, SHUT_RD);
}

void chen::bsd::socket::shutdownWrite() noexcept
{
    ::shutdown(this->_fd, SHUT_WR);
}

void chen::bsd::socket::close() noexcept
{
    if (!this->valid())
        return;

    ::close(this->_fd);
    this->_fd = socket::invalid_handle;
}

void chen::bsd::socket::reset()
{
    this->close();

    if (!this->_domain)
        throw std::runtime_error("socket: reset failed because domain is unknown");

    if ((this->_fd = ::socket(this->_domain, this->_type, this->_protocol)) < 0)
        throw std::system_error(sys::error(), "socket: failed to create socket");
}

// property
chen::bsd::endpoint chen::bsd::socket::sock() const noexcept
{
    bsd::endpoint ep;
    ep.size = sizeof(ep.addr);

    ::getsockname(this->_fd, (struct ::sockaddr*)&ep.addr, &ep.size);

    return ep;
}

chen::bsd::endpoint chen::bsd::socket::peer() const noexcept
{
    bsd::endpoint ep;
    ep.size = sizeof(ep.addr);

    ::getpeername(this->_fd, (struct ::sockaddr*)&ep.addr, &ep.size);

    return ep;
}

bool chen::bsd::socket::nonblocking() const noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return (flag & O_NONBLOCK) == O_NONBLOCK;
}

bool chen::bsd::socket::nonblocking(bool enable) noexcept
{
    auto flag = ::fcntl(this->_fd, F_GETFL, 0);
    if (flag < 0)
        return false;

    return !::fcntl(this->_fd, F_SETFL, enable ? (flag | O_NONBLOCK) : (flag & ~O_NONBLOCK));
}

chen::bsd::option chen::bsd::socket::option() noexcept
{
    return bsd::option(this->_fd);
}

bool chen::bsd::socket::valid() const noexcept
{
    return this->_fd >= 0;
}

chen::bsd::socket::operator bool() const noexcept
{
    return this->valid();
}

chen::socket_t chen::bsd::socket::native() const noexcept
{
    return this->_fd;
}