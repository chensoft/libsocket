/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "so_socket_unix.hpp"
#include <socket/tcp/tcp_socket.hpp>
#include <socket/so/so_error.hpp>
#include <chen/chen.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// socket
chen::tcp::socket::socket()
: chen::so::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)
{

}

socket::socket(void *so)
: chen::so::socket(so, PF_INET, SOCK_STREAM, IPPROTO_TCP)
{

}

// send & recv
void chen::tcp::socket::send(const void *data, std::size_t size, float timeout)
{
    if (!this->_impl->_socket)
        throw so::error("tcp: socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1)
        throw so::error_send("tcp: " + chen::sys::error());

    auto ret = ::send(this->_impl->_socket, data, size, 0);

    // timeout is also an error when send, because the data sent failure
    if (ret == -1)
        throw so::error_send("tcp: " + chen::sys::error());
    else if (ret != size)
        throw so::error_send("tcp: send packet length error");
}

std::size_t chen::tcp::socket::recv(void *data, std::size_t size, float timeout)
{
    if (!this->_impl->_socket)
        throw so::error("tcp: socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1)
        throw so::error_recv("tcp: " + chen::sys::error());

    auto ret = ::recv(this->_impl->_socket, data, size, 0);

    if (ret <= 0)
    {
        // errno will be EAGAIN if timeout
        // errno will be EBADF if call shutdown and close
        if (!errno || (errno == EAGAIN) || (errno == EBADF))
        {
            // timeout or non-blocking
            return 0;
        }
        else
        {
            throw so::error_recv("tcp: " + chen::sys::error());
        }
    }
    else
    {
        return static_cast<std::size_t>(ret);
    }
}