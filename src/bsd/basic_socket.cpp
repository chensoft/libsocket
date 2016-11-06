/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/basic_socket.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// socket
chen::basic_socket::basic_socket(std::nullptr_t) noexcept
{
}

chen::basic_socket::basic_socket(socket_t fd) noexcept
{
    this->reset(fd);
}

chen::basic_socket::basic_socket(int family, int type, int protocol)
{
    this->reset(family, type, protocol);
}

chen::basic_socket::basic_socket(basic_socket &&o) noexcept
{
    *this = std::move(o);
}

chen::basic_socket& chen::basic_socket::operator=(basic_socket &&o) noexcept
{
    if (this == &o)
        return *this;

    if (this->_fd != invalid_socket)
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

chen::basic_socket::~basic_socket() noexcept
{
    if (this->_fd != invalid_socket)
        this->close();
}

// reset
void chen::basic_socket::reset()
{
    if (this->_fd != invalid_socket)
        this->close();

    if (!this->_family)
        throw std::runtime_error("socket: reset failed because family is unknown");

    if ((this->_fd = ::socket(this->_family, this->_type, this->_protocol)) < 0)
        throw std::system_error(sys::error(), "socket: failed to create socket");
}

void chen::basic_socket::reset(socket_t fd) noexcept
{
    if (this->_fd != invalid_socket)
        this->close();

    this->_fd       = fd;
    this->_family   = 0;
    this->_type     = this->option().type();
    this->_protocol = 0;
}

void chen::basic_socket::reset(int family, int type, int protocol)
{
    this->_family   = family;
    this->_type     = type;
    this->_protocol = protocol;

    this->reset();
}

// connection
std::error_code chen::basic_socket::connect(const basic_address &addr) noexcept
{
    return !::connect(this->_fd, (struct ::sockaddr*)&addr.addr, addr.size) ? std::error_code() : sys::error();
}

std::error_code chen::basic_socket::bind(const basic_address &addr) noexcept
{
    return !::bind(this->_fd, (struct ::sockaddr*)&addr.addr, addr.size) ? std::error_code() : sys::error();
}

std::error_code chen::basic_socket::listen(int backlog) noexcept
{
    if (backlog <= 0)
        backlog = SOMAXCONN;

    return !::listen(this->_fd, backlog) ? std::error_code() : sys::error();
}

chen::basic_socket chen::basic_socket::accept() noexcept
{
    socket_t fd = invalid_socket;

    if ((fd = ::accept(this->_fd, nullptr, nullptr)) < 0)
        return nullptr;

    return basic_socket(fd);
}

chen::basic_socket chen::basic_socket::accept(basic_address &addr) noexcept
{
    socket_t fd = invalid_socket;

    if ((fd = ::accept(this->_fd, (struct ::sockaddr*)&addr.addr, &addr.size)) < 0)
        return nullptr;

    return basic_socket(fd);
}

// transmission
chen::ssize_t chen::basic_socket::recv(void *data, std::size_t size, int flags) noexcept
{
    return ::recv(this->_fd, (char*)data, size, flags);
}

chen::ssize_t chen::basic_socket::recvfrom(void *data, std::size_t size, basic_address &addr, int flags) noexcept
{
    return ::recvfrom(this->_fd, (char*)data, size, flags, (struct ::sockaddr*)&addr.addr, &addr.size);
}

chen::ssize_t chen::basic_socket::send(const void *data, std::size_t size, int flags) noexcept
{
    return ::send(this->_fd, (char*)data, size, flags);
}

chen::ssize_t chen::basic_socket::sendto(const void *data, std::size_t size, const basic_address &addr, int flags) noexcept
{
    return ::sendto(this->_fd, (char*)data, size, flags, (struct ::sockaddr*)&addr.addr, addr.size);
}

// property
chen::basic_address chen::basic_socket::peer() const noexcept
{
    basic_address addr;
    ::getpeername(this->_fd, (struct ::sockaddr*)&addr.addr, &addr.size);
    return addr;
}

chen::basic_address chen::basic_socket::sock() const noexcept
{
    basic_address addr;
    ::getsockname(this->_fd, (struct ::sockaddr*)&addr.addr, &addr.size);
    return addr;
}

chen::basic_option chen::basic_socket::option() noexcept
{
    return basic_option(*this);
}

bool chen::basic_socket::valid() const noexcept
{
    return this->_fd != invalid_socket;
}

chen::basic_socket::operator bool() const noexcept
{
    return this->valid();
}

chen::socket_t chen::basic_socket::native() const noexcept
{
    return this->_fd;
}

int chen::basic_socket::family() const noexcept
{
    return this->_family;
}

int chen::basic_socket::type() const noexcept
{
    return this->_type;
}

int chen::basic_socket::protocol() const noexcept
{
    return this->_protocol;
}