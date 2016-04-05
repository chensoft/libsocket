/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "tcp_socket_unix.hpp"
#include <chen/net/tcp/tcp_client.hpp>
#include <chen/net/tcp/tcp_error.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// client
void client::connect(const std::string &host, std::uint16_t port, float timeout)
{
    if (!this->_remote_host.empty())
        this->build();

    struct sockaddr_in in;
    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(host.c_str());
    in.sin_port        = htons(port);

    if (::connect(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in)) == -1)
        throw error_bind(std::strerror(errno));

    // todo resolve host to ip address
    this->_remote_host = host;
    this->_remote_addr = host;
    this->_remote_port = port;

    socklen_t len = sizeof(in);
    ::getsockname(this->_impl->_socket, (struct sockaddr*)&in, &len);

    this->_local_addr = ::inet_ntoa(in.sin_addr);
    this->_local_port = ntohs(in.sin_port);

    this->_connected = true;
}

#endif