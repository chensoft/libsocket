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
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

using namespace chen;
using namespace chen::so;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// client
bool client::connect(const std::string &addr, std::uint16_t port, float timeout)
{
    if (!this->_impl->_socket || this->remotePort())
        this->build();

    struct sockaddr_in in;
    ::memset(&in, 0, sizeof(in));

    in.sin_family      = AF_INET;
    in.sin_addr.s_addr = ::inet_addr(addr.c_str());
    in.sin_port        = htons(port);

    if (timeout)
    {
        // make non-blocking first
        auto flag = ::fcntl(this->_impl->_socket, F_GETFL);
        ::fcntl(this->_impl->_socket, F_SETFL, flag | O_NONBLOCK);

        ::connect(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in));

        if (errno != EINPROGRESS)
        {
            ::fcntl(this->_impl->_socket, F_SETFL, flag);
            throw error_connect(std::strerror(errno));
        }

        struct timeval tv;
        tv.tv_sec  = static_cast<int>(timeout);
        tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1000000);

        fd_set set;

        FD_ZERO(&set);
        FD_SET(this->_impl->_socket, &set);

        auto ret = ::select(this->_impl->_socket + 1, nullptr, &set, nullptr, &tv);

        ::fcntl(this->_impl->_socket, F_SETFL, flag);

        if ((ret == 1) && !errno)
        {
            this->_connected = true;
        }
        else if (!ret || (errno == EBADF) || (errno == EINPROGRESS))
        {
            // timeout or shutdown or close or cancelled when connect
            this->_connected = false;
        }
        else
        {
            throw error_accept(std::strerror(errno));
        }
    }
    else
    {
        if (::connect(this->_impl->_socket, (struct sockaddr*)&in, sizeof(in)) == -1)
            throw error_connect(std::strerror(errno));

        this->_connected = true;
    }

    this->_recent_addr = addr;
    this->_recent_port = port;

    return this->_connected;
}

bool client::isBlocking() const
{
    auto flag = ::fcntl(this->_impl->_socket, F_GETFL);
    return (flag == -1) ? false : ((flag & O_NONBLOCK) == 0);
}

bool client::isNonBlocking() const
{
    auto flag = ::fcntl(this->_impl->_socket, F_GETFL);
    return (flag == -1) ? false : ((flag & O_NONBLOCK) != 0);
}

#endif