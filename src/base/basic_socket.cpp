/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_socket.hpp>
#include <socket/core/ioctl.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// basic_socket
const int chen::basic_socket::FlagOutOfBand  = MSG_OOB;
const int chen::basic_socket::FlagPeek       = MSG_PEEK;
const int chen::basic_socket::FlagDoNotRoute = MSG_DONTROUTE;
const int chen::basic_socket::FlagWaitAll    = MSG_WAITALL;

chen::basic_socket::basic_socket(std::nullptr_t) noexcept
{
}

chen::basic_socket::basic_socket(handle_t fd) noexcept
{
    this->reset(fd);
}

chen::basic_socket::basic_socket(handle_t fd, int family, int type, int protocol) noexcept
{
    this->reset(fd, family, type, protocol);
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

    this->reset(o.transfer(), o.family(), o.type(), o.protocol());

    o._family   = 0;
    o._type     = 0;
    o._protocol = 0;

    return *this;
}

// reset
void chen::basic_socket::reset()
{
    if (!this->_family)
        throw std::runtime_error("socket: reset failed because family is unknown");

#ifdef __linux__
    this->change(::socket(this->_family, this->_type | SOCK_CLOEXEC, this->_protocol));
#else
    this->change(::socket(this->_family, this->_type, this->_protocol));
    ioctl::cloexec(this->native(), true);
#endif

    if (!this->valid())
        throw std::system_error(sys::error(), "socket: failed to create socket");

#ifdef SO_NOSIGPIPE
    // this macro is defined on Unix to prevent SIGPIPE on this socket
    this->option().set(SOL_SOCKET, SO_NOSIGPIPE, 1);
#endif
}

void chen::basic_socket::reset(int family, int type, int protocol)
{
    this->_family   = family;
    this->_type     = type;
    this->_protocol = protocol;

    this->reset();
}

void chen::basic_socket::reset(handle_t fd) noexcept
{
    this->reset(fd, 0, 0, 0);
    this->_type = this->option().type();
}

void chen::basic_socket::reset(handle_t fd, int family, int type, int protocol) noexcept
{
    this->change(fd);

    this->_family   = family;
    this->_type     = type;
    this->_protocol = protocol;

#ifdef SO_NOSIGPIPE
    // this macro is defined on Unix to prevent SIGPIPE on this socket
    this->option().set(SOL_SOCKET, SO_NOSIGPIPE, 1);
#endif
}

// connection
std::error_code chen::basic_socket::connect(const basic_address &addr) noexcept
{
    return !::connect(this->native(), (::sockaddr*)&addr.addr, addr.size) ? std::error_code() : sys::error();
}

std::error_code chen::basic_socket::bind(const basic_address &addr) noexcept
{
    return !::bind(this->native(), (::sockaddr*)&addr.addr, addr.size) ? std::error_code() : sys::error();
}

std::error_code chen::basic_socket::listen() noexcept
{
    return this->listen(SOMAXCONN);
}

std::error_code chen::basic_socket::listen(int backlog) noexcept
{
    return !::listen(this->native(), backlog) ? std::error_code() : sys::error();
}

chen::basic_socket chen::basic_socket::accept() noexcept
{
    handle_t fd = invalid_handle;

    if ((fd = ::accept(this->native(), nullptr, nullptr)) == invalid_handle)
        return nullptr;

    ioctl::cloexec(fd, true);

    return basic_socket(fd);
}

// transmission
chen::ssize_t chen::basic_socket::recv(void *data, std::size_t size) noexcept
{
    return this->recv(data, size, 0);
}

chen::ssize_t chen::basic_socket::recv(void *data, std::size_t size, int flags) noexcept
{
#ifdef MSG_NOSIGNAL
    // this macro is defined on Linux to prevent SIGPIPE on this socket
    flags |= MSG_NOSIGNAL;
#endif

    return ::recv(this->native(), (char*)data, size, flags);
}

chen::ssize_t chen::basic_socket::recvfrom(void *data, std::size_t size) noexcept
{
    basic_address addr;
    return this->recvfrom(data, size, addr, 0);
}

chen::ssize_t chen::basic_socket::recvfrom(void *data, std::size_t size, basic_address &addr) noexcept
{
    return this->recvfrom(data, size, addr, 0);
}

chen::ssize_t chen::basic_socket::recvfrom(void *data, std::size_t size, basic_address &addr, int flags) noexcept
{
#ifdef MSG_NOSIGNAL
    // this macro is defined on Linux to prevent SIGPIPE on this socket
    flags |= MSG_NOSIGNAL;
#endif

    return ::recvfrom(this->native(), (char*)data, size, flags, (::sockaddr*)&addr.addr, &addr.size);
}

chen::ssize_t chen::basic_socket::send(const void *data, std::size_t size) noexcept
{
    return this->send(data, size, 0);
}

chen::ssize_t chen::basic_socket::send(const void *data, std::size_t size, int flags) noexcept
{
#ifdef MSG_NOSIGNAL
    // this macro is defined on Linux to prevent SIGPIPE on this socket
    flags |= MSG_NOSIGNAL;
#endif

    return ::send(this->native(), (char*)data, size, flags);
}

chen::ssize_t chen::basic_socket::sendto(const void *data, std::size_t size) noexcept
{
    basic_address addr;
    return this->sendto(data, size, addr, 0);
}

chen::ssize_t chen::basic_socket::sendto(const void *data, std::size_t size, const basic_address &addr) noexcept
{
    return this->sendto(data, size, addr, 0);
}

chen::ssize_t chen::basic_socket::sendto(const void *data, std::size_t size, const basic_address &addr, int flags) noexcept
{
#ifdef MSG_NOSIGNAL
    // this macro is defined on Linux to prevent SIGPIPE on this socket
    flags |= MSG_NOSIGNAL;
#endif

    return ::sendto(this->native(), (char*)data, size, flags, (::sockaddr*)&addr.addr, addr.size);
}

// cleanup
void chen::basic_socket::shutdown(Shutdown type) noexcept
{
#ifdef _WIN32
    #define SHUT_RD   SD_RECEIVE
    #define SHUT_WR   SD_SEND
    #define SHUT_RDWR SD_BOTH
#endif

    switch (type)
    {
        case Shutdown::Read:
            ::shutdown(this->native(), SHUT_RD);
            break;

        case Shutdown::Write:
            ::shutdown(this->native(), SHUT_WR);
            break;

        case Shutdown::Both:
            ::shutdown(this->native(), SHUT_RDWR);
            break;
    }
}

// property
chen::basic_address chen::basic_socket::peer() const noexcept
{
    basic_address addr;
    ::getpeername(this->native(), (::sockaddr*)&addr.addr, &addr.size);
    return addr;
}

chen::basic_address chen::basic_socket::sock() const noexcept
{
    basic_address addr;
    ::getsockname(this->native(), (::sockaddr*)&addr.addr, &addr.size);
    return addr;
}

std::error_code chen::basic_socket::nonblocking(bool enable) noexcept
{
    return ioctl::nonblocking(this->native(), enable);
}

chen::basic_option chen::basic_socket::option() noexcept
{
    return basic_option(*this);
}

bool chen::basic_socket::valid() const noexcept
{
    return this->native() != invalid_handle;
}

chen::basic_socket::operator bool() const noexcept
{
    return this->valid();
}

std::size_t chen::basic_socket::available() const noexcept
{
    return ioctl::available(this->native());
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