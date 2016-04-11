/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include "so_socket_unix.hpp"
#include <chen/net/tcp/tcp_client.hpp>
#include <chen/net/so/so_error.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::so;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// client
void client::connect(const std::string &addr, std::uint16_t port, float timeout)
{
    if (!this->_impl || this->remotePort())
        this->build();

    struct sockaddr_in in;
    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    if (::connect(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in)) == -1)
        throw error_bind(std::strerror(errno));

    this->_recent_addr = addr;
    this->_recent_port = port;
    this->_connected   = true;
}

#endif