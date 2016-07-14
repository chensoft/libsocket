/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include "so_socket.unix.hpp"
#include <socket/udp/udp_server.hpp>
#include <socket/so/so_error.hpp>
#include <chen/chen.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace chen;
using namespace chen::so;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server
void server::bind(const std::string &addr, std::uint16_t port)
{
    // check already bind
    if (!this->_impl->_socket || this->localPort())
        this->build();  // rebuild

    // bind address and port
    struct sockaddr_in in;
    ::memset(&in, 0, sizeof(in));

    struct sockaddr_in6 dest_sin6;

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    if (::bind(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in)) == -1)
        throw error_bind("udp: " + chen::sys::error());
}

#endif