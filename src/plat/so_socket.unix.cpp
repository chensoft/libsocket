/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include "so_socket.unix.hpp"
#include <socket/so/so_error.hpp>
#include <chen/chen.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace chen;
using namespace chen::so;

// -----------------------------------------------------------------------------
// socket
socket::socket(int domain, int type, int protocol) : _impl(new socket::impl)
{
    this->_impl->_domain   = domain;
    this->_impl->_type     = type;
    this->_impl->_protocol = protocol;

    this->build();
}

socket::socket(void *so, int domain, int type, int protocol) : _impl(new socket::impl)
{
    this->_impl->_socket   = *static_cast<int*>(so);
    this->_impl->_domain   = domain;
    this->_impl->_type     = type;
    this->_impl->_protocol = protocol;
}

socket::~socket()
{
    if (this->_impl->_socket)
        this->close();
}

socket::socket(socket &&o)
{
    *this = std::move(o);
}

so::socket& socket::operator=(socket &&o)
{
    if (this == &o)
        return *this;

    this->close();

    this->_impl = std::move(o._impl);
    o._impl.reset(new socket::impl);

    return *this;
}

// close
void socket::close()
{
    if (!this->_impl || !this->_impl->_socket)
        return;

    ::close(this->_impl->_socket);
    this->_impl->_socket = 0;
}

void socket::shutdown(Shutdown flag)
{
    if (!this->_impl->_socket)
        return;

    if (flag == Shutdown::Read)
        ::shutdown(this->_impl->_socket, SHUT_RD);
    else if (flag == Shutdown::Write)
        ::shutdown(this->_impl->_socket, SHUT_WR);
    else if (flag == Shutdown::Both)
        ::shutdown(this->_impl->_socket, SHUT_RDWR);
}

// remote
std::string socket::remoteAddr() const
{
    if (!this->_impl->_socket)
        return "";

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    ::memset(&in, 0, sizeof(in));

    if (::getpeername(this->_impl->_socket, (struct sockaddr*)&in, &len) == -1)
        return "";
    else
        return ::inet_ntoa(in.sin_addr);
}

std::uint16_t socket::remotePort() const
{
    if (!this->_impl->_socket)
        return 0;

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    ::memset(&in, 0, sizeof(in));

    if (::getpeername(this->_impl->_socket, (struct sockaddr*)&in, &len) == -1)
        return 0;
    else
        return ntohs(in.sin_port);
}

// local
std::string socket::localAddr() const
{
    if (!this->_impl->_socket)
        return "";

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    ::memset(&in, 0, sizeof(in));

    if (::getsockname(this->_impl->_socket, (struct sockaddr*)&in, &len) == -1)
        return "";
    else
        return ::inet_ntoa(in.sin_addr);
}

std::uint16_t socket::localPort() const
{
    if (!this->_impl->_socket)
        return 0;

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    ::memset(&in, 0, sizeof(in));

    if (::getsockname(this->_impl->_socket, (struct sockaddr*)&in, &len) == -1)
        return 0;
    else
        return ntohs(in.sin_port);
}

// build
void socket::build()
{
    if (this->_impl->_socket)
        this->close();

    auto sock = ::socket(this->_impl->_domain, this->_impl->_type, this->_impl->_protocol);

    if (sock == -1)
        throw error_build("so: " + chen::sys::error());
    else
        this->_impl->_socket = sock;
}

#endif