/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "udp_socket_unix.h"
#include <chen/net/udp/udp_error.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>

using namespace chen;
using namespace chen::udp;

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

void socket::send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port)
{
    if (!this->_impl)
        throw error("udp: socket invalid");

    struct sockaddr_in in;

    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    auto ret = ::sendto(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, sizeof(in));

    if (ret == -1)
        throw error_send(std::strerror(errno));
    else if (ret != size)
        throw error_send("udp: send packet length is error");
}

void socket::recv(std::uint8_t *data, std::size_t &size, std::string &addr, std::uint16_t &port)
{
    if (!this->_impl)
        throw error("udp: socket invalid");

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    auto ret = ::recvfrom(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, &len);

    if (ret == -1)
        throw error_recv(std::strerror(errno));

    size = static_cast<std::size_t>(ret);
    addr = ::inet_ntoa(in.sin_addr);
    port = ntohs(in.sin_port);
}

void socket::close()
{
    if (this->_impl)
    {
        ::close(this->_impl->_socket);
        this->_impl.reset();
    }
}

void socket::build()
{
    if (this->_impl)
        this->close();

    this->_impl.reset(new socket::impl);

    auto sock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == -1)
        throw error_build(std::strerror(errno));
    else
        this->_impl->_socket = sock;
}

#endif  // CHEN_OS_UNIX