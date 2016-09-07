/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_socket.hpp>

namespace chen
{
    namespace tcp
    {
        /**
         * A non-blocking event driven tcp base class
         */
        class basic : protected socket
        {
        public:
            basic(socket_t fd);
            basic(int family, int type, int protocol = 0);
            basic(ip::address::Type family, int type, int protocol = 0);

        public:
            using socket::local;
            using socket::remote;

            using socket::nonblocking;

            using socket::native;
        };
    }
}