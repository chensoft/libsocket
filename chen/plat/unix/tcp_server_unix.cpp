/**
 * Created by Jian Chen
 * @since  2016.04.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "so_socket_unix.hpp"
#include <chen/net/tcp/tcp_server.hpp>
#include <chen/net/so/so_error.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::so;
using namespace chen::tcp;

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

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    if (::bind(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in)) == -1)
        throw error_bind(std::strerror(errno));
}

void server::listen()
{
    if (::listen(this->_impl->_socket, SOMAXCONN) == -1)
        throw error_bind(std::strerror(errno));
}

std::unique_ptr<chen::tcp::conn> server::accept()
{
    struct sockaddr_in in;
    socklen_t len = sizeof(in);

    auto so = ::accept(this->_impl->_socket, (struct sockaddr*)&in, &len);
    if (so == -1)
        throw error_bind(std::strerror(errno));

    return std::unique_ptr<chen::tcp::conn>(new chen::tcp::conn(so));
}

#endif