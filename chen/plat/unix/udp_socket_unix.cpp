/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "so_socket_unix.hpp"
#include <chen/net/udp/udp_socket.hpp>
#include <chen/net/so/so_error.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::so;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// socket
chen::udp::socket::socket()
{
    this->build();
}

void chen::udp::socket::send(const void *data, std::size_t size, const std::string &addr, std::uint16_t port, float timeout)
{
    if (!this->_impl)
        throw error("udp socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1)
        throw error_send(std::strerror(errno));

    struct sockaddr_in in;

    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    auto ret = ::sendto(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, sizeof(in));

    // timeout is also an error when send, because the data sent failure
    if (ret == -1)
        throw error_send(std::strerror(errno));
    else if (ret != size)
        throw error_send("udp send packet length error");
}

std::size_t chen::udp::socket::recv(void *data, std::size_t size, std::string &addr, std::uint16_t &port, float timeout)
{
    if (!this->_impl)
        throw error("udp socket invalid");

    struct timeval tv;
    tv.tv_sec  = static_cast<int>(timeout);
    tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

    if (::setsockopt(this->_impl->_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1)
        throw error_recv(std::strerror(errno));

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
            throw error_recv(std::strerror(errno));
        }
    }
    else
    {
        addr = ::inet_ntoa(in.sin_addr);
        port = ntohs(in.sin_port);
        return static_cast<std::size_t>(ret);
    }
}

void chen::udp::socket::build()
{
    so::socket::build(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

#endif  // CHEN_OS_UNIX