/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://man7.org/linux/man-pages/man7/socket.7.html
 */
#pragma once

#include <socket/net/net_socket.hpp>

namespace chen
{
    namespace net
    {
        namespace option
        {
            // -----------------------------------------------------------------
            // basic
            class basic
            {
            public:
                static int optionInt(socket_t fd, int level, int name);
            };


            // -----------------------------------------------------------------
            // SO_ACCEPTCONN(read-only, determine whether the socket is a listening socket)
            class accept_conn : public basic
            {
            public:
                /**
                 * Construct by getsockopt or bool value
                 */
                explicit accept_conn(const socket &sock);
                explicit accept_conn(bool val);

            public:
                bool listening = false;
            };


            // -----------------------------------------------------------------
            // SO_BROADCAST(enable broadcast for datagram socket)
            class broadcast : public basic
            {
            public:
                explicit broadcast(const socket &sock);
                explicit broadcast(bool val);

            public:
                /**
                 * Call setsockopt to apply the value to socket
                 */
                bool apply(socket &sock);

            public:
                bool enable = false;
            };
        }
    }
}