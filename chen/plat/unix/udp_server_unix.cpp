/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include <chen/net/udp/udp_server.h>
#include <chen/net/udp/udp_error.h>
#include <chen/net/ip/ip_addr.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include "udp_socket_unix.h"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server
void server::bind(const std::string &addr, std::uint16_t port)
{
    this->build();

    struct sockaddr_in in;

    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = htonl(ip::address_v4::to_integer(addr));
    in.sin_port        = htons(port);

    if (::bind(this->_impl->_socket, (sockaddr*)&in, sizeof(in)) == -1)
        throw error_bind(std::strerror(errno));
}

#endif  // CHEN_OS_UNIX