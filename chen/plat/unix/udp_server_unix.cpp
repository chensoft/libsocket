/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "udp_socket_unix.h"
#include <chen/net/udp/udp_server.h>
#include <chen/net/udp/udp_error.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server
void server::bind(const std::string &addr, std::uint16_t port)
{
    // todo @@ how to determine socket is already bind, getsockname?
    struct sockaddr_in in;

    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    if (::bind(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in)) == -1)
        throw error_bind(std::strerror(errno));

    this->_addr = addr;
    this->_port = port;
}

#endif