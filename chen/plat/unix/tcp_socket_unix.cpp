/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "tcp_socket_unix.hpp"
#include <chen/net/tcp/tcp_error.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// socket
socket::socket()
{
    this->build();
}

socket::~socket()
{
    if (this->_impl)
        this->close();
}

void socket::send(const void *data, std::size_t size, float timeout)
{
    if (!this->_impl)
        throw error("tcp socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1)
        throw error_send(std::strerror(errno));

    auto ret = ::send(this->_impl->_socket, data, size, 0);

    // timeout is also an error when send, because the data sent failure
    if (ret == -1)
        throw error_send(std::strerror(errno));
    else if (ret != size)
        throw error_send("tcp send packet length is error");
}

void socket::recv(void *data, std::size_t &size, float timeout)
{
    if (!this->_impl)
        throw error("tcp socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1)
        throw error_recv(std::strerror(errno));

    auto ret = ::recv(this->_impl->_socket, data, size, 0);

    if (ret == -1)
    {
        // don't treat timeout as an error
        if (errno == EAGAIN)
        {
            // timeout or non-blocking
            size = 0;
        }
        else
        {
            throw error_recv(std::strerror(errno));
        }
    }
    else
    {
        size = static_cast<std::size_t>(ret);
    }
}

void socket::close()
{
    if (this->_impl)
    {
        ::close(this->_impl->_socket);
        this->_impl.reset();
    }
}

void socket::shutdown(Shutdown flag)
{
    if (!this->_impl)
        return;

    if (flag == Shutdown::Read)
        ::shutdown(this->_impl->_socket, SHUT_RD);
    else if (flag == Shutdown::Write)
        ::shutdown(this->_impl->_socket, SHUT_WR);
    else if (flag == Shutdown::Both)
        ::shutdown(this->_impl->_socket, SHUT_RDWR);
}

void socket::build()
{
    if (this->_impl)
        this->close();

    this->_impl.reset(new socket::impl);

    auto sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == -1)
        throw error_build(std::strerror(errno));
    else
        this->_impl->_socket = sock;
}

#endif  // CHEN_OS_UNIX