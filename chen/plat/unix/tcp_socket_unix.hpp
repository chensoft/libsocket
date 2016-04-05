/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef CHEN_OS_UNIX

#include <chen/net/tcp/tcp_socket.hpp>

namespace chen
{
    namespace tcp
    {
        struct socket::impl
        {
            int _socket = 0;
        };
    }
}

#endif  // CHEN_OS_UNIX