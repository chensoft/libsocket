/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include "so_socket.unix.hpp"
#include <socket/udp/udp_socket.hpp>
#include <socket/so/so_error.hpp>
#include <chen/chen.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// socket
chen::udp::socket::socket()
: chen::so::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)
{

}

socket::socket(void *so)
: chen::so::socket(so, PF_INET, SOCK_DGRAM, IPPROTO_UDP)
{

}

void chen::udp::socket::send(const void *data, std::size_t size, const std::string &addr, std::uint16_t port, float timeout)
{
    if (!this->_impl->_socket)
        throw so::error("udp: socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1)
        throw so::error_send("udp: " + chen::sys::error());

    struct sockaddr_in in;

    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    auto ret = ::sendto(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, sizeof(in));

    // timeout is also an error when send, because the data sent failure
    if (ret == -1)
        throw so::error_send("udp: " + chen::sys::error());
    else if (ret != size)
        throw so::error_send("udp: send packet length error");
}

std::size_t chen::udp::socket::recv(void *data, std::size_t size, std::string &addr, std::uint16_t &port, float timeout)
{
    if (!this->_impl->_socket)
        throw so::error("udp: socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1)
        throw so::error_recv("udp: " + chen::sys::error());

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    auto ret = ::recvfrom(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, &len);

    if (ret <= 0)
    {
        // errno will be EAGAIN if timeout
        // errno will be EBADF if call shutdown and close
        if (!errno || (errno == EAGAIN) || (errno == EBADF))
        {
            // timeout or non-blocking
            addr = "";
            port = 0;
            return 0;
        }
        else
        {
            throw so::error_recv("udp: " + chen::sys::error());
        }
    }
    else
    {
        addr = ::inet_ntoa(in.sin_addr);
        port = ntohs(in.sin_port);
        return static_cast<std::size_t>(ret);
    }
}

#endif