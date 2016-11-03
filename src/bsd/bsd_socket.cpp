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
const int chen::bsd::socket::FlagOutOfBand  = MSG_OOB;
const int chen::bsd::socket::FlagPeek       = MSG_PEEK;
const int chen::bsd::socket::FlagDoNotRoute = MSG_DONTROUTE;
const int chen::bsd::socket::FlagWaitAll    = MSG_WAITALL;

chen::bsd::socket::socket(std::nullptr_t) noexcept
{
}

chen::bsd::socket::socket(socket_t fd) noexcept
{
    this->reset(fd);
}

chen::bsd::socket::socket(int family, int type, int protocol)
{
    this->reset(family, type, protocol);
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
    this->_family   = o._family;
    this->_type     = o._type;
    this->_protocol = o._protocol;

    o._fd       = invalid_socket;
    o._family   = 0;
    o._type     = 0;
    o._protocol = 0;

    return *this;
}

chen::bsd::socket::~socket() noexcept
{
    this->close();
}

// reset
void chen::bsd::socket::reset()
{
    this->close();

    if (!this->_family)
        throw std::runtime_error("socket: reset failed because family is unknown");

    if ((this->_fd = ::socket(this->_family, this->_type, this->_protocol)) < 0)
        throw std::system_error(sys::error(), "socket: failed to create socket");
}

void chen::bsd::socket::reset(socket_t fd) noexcept
{
    this->close();

    this->_fd       = fd;
    this->_family   = 0;
    this->_type     = this->option().type();
    this->_protocol = 0;
}

void chen::bsd::socket::reset(int family, int type, int protocol)
{
    this->_family   = family;
    this->_type     = type;
    this->_protocol = protocol;

    this->reset();
}

// connection
std::error_code chen::bsd::socket::connect(const bsd::endpoint &ep) noexcept
{
    return !::connect(this->_fd, (struct ::sockaddr*)&ep.addr, ep.size) ? std::error_code() : sys::error();
}

std::error_code chen::bsd::socket::bind(const bsd::endpoint &ep) noexcept
{
    return !::bind(this->_fd, (struct ::sockaddr*)&ep.addr, ep.size) ? std::error_code() : sys::error();
}

std::error_code chen::bsd::socket::listen(int backlog) noexcept
{
    if (backlog <= 0)
        backlog = SOMAXCONN;

    return !::listen(this->_fd, backlog) ? std::error_code() : sys::error();
}

chen::bsd::socket chen::bsd::socket::accept() noexcept
{
    socket_t fd = 0;

    if ((fd = ::accept(this->_fd, nullptr, nullptr)) < 0)
        return nullptr;

    return socket(fd);
}

chen::bsd::socket chen::bsd::socket::accept(bsd::endpoint &ep) noexcept
{
    socket_t fd = 0;

    if ((fd = ::accept(this->_fd, (struct ::sockaddr*)&ep.addr, &ep.size)) < 0)
        return nullptr;

    return socket(fd);
}

// transmission
chen::ssize_t chen::bsd::socket::recv(void *data, std::size_t size, int flags) noexcept
{
    return ::recv(this->_fd, (char*)data, size, flags);
}

chen::ssize_t chen::bsd::socket::recvfrom(void *data, std::size_t size, bsd::endpoint &ep, int flags) noexcept
{
    return ::recvfrom(this->_fd, (char*)data, size, flags, (struct ::sockaddr*)&ep.addr, &ep.size);
}

chen::ssize_t chen::bsd::socket::send(const void *data, std::size_t size, int flags) noexcept
{
    return ::send(this->_fd, (char*)data, size, flags);
}

chen::ssize_t chen::bsd::socket::sendto(const void *data, std::size_t size, const bsd::endpoint &ep, int flags) noexcept
{
    return ::sendto(this->_fd, (char*)data, size, flags, (struct ::sockaddr*)&ep.addr, ep.size);
}

// property
chen::bsd::endpoint chen::bsd::socket::peer() const noexcept
{
    bsd::endpoint ep;
    ::getpeername(this->_fd, (struct ::sockaddr*)&ep.addr, &ep.size);
    return ep;
}

chen::bsd::endpoint chen::bsd::socket::sock() const noexcept
{
    bsd::endpoint ep;
    ::getsockname(this->_fd, (struct ::sockaddr*)&ep.addr, &ep.size);
    return ep;
}

chen::bsd::option chen::bsd::socket::option() noexcept
{
    return bsd::option(*this);
}

bool chen::bsd::socket::valid() const noexcept
{
    return this->_fd != invalid_socket;
}

chen::bsd::socket::operator bool() const noexcept
{
    return this->valid();
}

chen::socket_t chen::bsd::socket::native() const noexcept
{
    return this->_fd;
}

int chen::bsd::socket::family() const noexcept
{
    return this->_family;
}

int chen::bsd::socket::type() const noexcept
{
    return this->_type;
}

int chen::bsd::socket::protocol() const noexcept
{
    return this->_protocol;
}