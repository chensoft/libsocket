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
    namespace udp
    {
        class basic : protected socket
        {
        public:
            using socket::local;
            using socket::remote;

            using socket::nonblocking;

            using socket::empty;
            using socket::operator bool;

            using socket::native;
        };
    }
}