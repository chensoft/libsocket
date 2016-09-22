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

chen::bsd::socket::socket(socket &&o) noexcept
{
    *this = std::move(o);
}

chen::bsd::socket& chen::bsd::socket::operator=(socket &&o) noexcept
{
    if (this == &o)
        return *this;

    this->close();

    this->_fd       = o._fd;
    this->_domain   = o._domain;
    this->_type     = o._type;
    this->_protocol = o._protocol;

    o._fd = socket::invalid_handle;

    return *this;
}

chen::bsd::socket::~socket() noexcept
{
    this->close();
}

// connection
std::error_code chen::bsd::socket::connect(const bsd::endpoint &ep) noexcept
{
    auto tmp = ep.get();
    return ::connect(this->_fd, (struct ::sockaddr*)&tmp, ep.len()) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::bsd::socket::bind(const bsd::endpoint &ep) noexcept
{
    auto tmp = ep.get();
    return ::bind(this->_fd, (struct ::sockaddr*)&tmp, ep.len()) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::bsd::socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

std::error_code chen::bsd::socket::listen(int backlog) noexcept
{
    return ::listen(this->_fd, backlog) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::bsd::socket::accept(socket_t &fd, bsd::endpoint &ep) noexcept
{
    struct ::sockaddr_storage tmp{};
    socklen_t len = 0;

    if ((fd = ::accept(this->_fd, (struct ::sockaddr*)&tmp, &len)) < 0)
        return sys::error();

    ep.set(tmp);

    return {};
}

// transmission
ssize_t chen::bsd::socket::send(const void *data, std::size_t size, int flags) noexcept
{
    return ::send(this->_fd, data, size, flags);
}

ssize_t chen::bsd::socket::send(const void *data, std::size_t size, const bsd::endpoint &ep, int flags) noexcept
{
    auto tmp = ep.get();
    return ::sendto(this->_fd, data, size, flags, (struct ::sockaddr*)&tmp, ep.len());
}

std::error_code chen::bsd::socket::recv(std::vector<std::uint8_t> &out, int flags) noexcept
{
    auto size = ::recv(this->_fd, out.data(), out.size(), flags);
    if (size < 0)
        return sys::error();

    out.resize(static_cast<std::size_t>(size));
    return {};
}

std::error_code chen::bsd::socket::recv(std::vector<std::uint8_t> &out, bsd::endpoint &ep, int flags) noexcept
{
    struct sockaddr_storage tmp{};
    socklen_t len = 0;

    auto size = ::recvfrom(this->_fd, out.data(), out.size(), flags, (struct sockaddr*)&tmp, &len);
    if (size < 0)
        return sys::error();

    ep.set(tmp);
    out.resize(static_cast<std::size_t>(size));

    return {};
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
    if (!this->_domain)
        throw std::runtime_error("socket: reset failed because domain is unknown");

    if ((this->_fd = ::socket(this->_domain, this->_type, this->_protocol)) < 0)
        throw std::system_error(sys::error(), "socket: failed to create socket");
}

// property
void chen::bsd::socket::sock(bsd::endpoint &ep) const noexcept
{
    struct sockaddr_storage tmp{};
    socklen_t len = sizeof(tmp);

    ::getsockname(this->_fd, (struct sockaddr*)&tmp, &len);

    ep.set(tmp);
}

void chen::bsd::socket::peer(bsd::endpoint &ep) const noexcept
{
    struct sockaddr_storage tmp{};
    socklen_t len = sizeof(tmp);

    ::getpeername(this->_fd, (struct sockaddr*)&tmp, &len);

    ep.set(tmp);
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