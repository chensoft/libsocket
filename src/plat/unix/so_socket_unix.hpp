/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef CHEN_OS_UNIX

#include <chen/net/so/so_socket.hpp>

namespace chen
{
    namespace so
    {
        struct socket::impl
        {
            int _socket   = 0;
            int _domain   = 0;
            int _type     = 0;
            int _protocol = 0;
        };
    }
}

#endif  // CHEN_OS_UNIX