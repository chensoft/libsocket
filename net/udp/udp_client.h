/**
 * Created by Jian Chen
 * @since  2015.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "udp_socket.h"

namespace chen
{
    namespace udp
    {
        class client : public socket
        {
        public:
            client();
        };
    }
}