/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#ifdef CHEN_OS_UNIX

#include <chen/net/udp/udp_socket.h>

namespace chen
{
    namespace udp
    {
        struct socket::impl
        {
            int _socket = 0;
        };
    }
}

#endif  // CHEN_OS_UNIX