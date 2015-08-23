/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "net/udp/udp_socket.h"
#include "net/udp/udp_error.h"
#include "net/ip/ip_addr.h"
#include "udp_socket_unix.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// socket
socket::socket()
{

}

socket::~socket()
{
    if (this->_impl)
        this->close();
}

std::size_t socket::send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port)
{
    if (!this->_impl)
        throw error_socket("socket invalid");

    struct sockaddr_in in;

    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = htonl(ip::address_v4::to_integer(addr));
    in.sin_port        = htons(port);

    socklen_t len = sizeof(in);

    auto ret = ::sendto(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, len);

    if (ret == -1)
        throw error_recv(std::strerror(errno));

    return static_cast<std::size_t>(ret);
}

std::size_t socket::recv(std::uint8_t *data, std::size_t size, std::string &addr, std::uint16_t &port)
{
    if (!this->_impl)
        throw error_socket("socket invalid");

    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    auto ret = ::recvfrom(this->_impl->_socket, data, size, 0, (struct sockaddr*)&in, &len);

    if (ret == -1)
        throw error_recv(std::strerror(errno));

    return static_cast<std::size_t>(ret);
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

    auto sock = ::socket(PF_INET, SOCK_DGRAM, 0);

    if (sock == -1)
        throw error_socket(std::strerror(errno));
    else
        this->_impl->_socket = sock;
}

#endif  // CHEN_OS_UNIX