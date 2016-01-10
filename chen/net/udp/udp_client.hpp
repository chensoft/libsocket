/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "udp_socket.hpp"

namespace chen
{
    namespace udp
    {
        class client : public socket
        {
        public:
            client();

        private:
            client(const client&) = delete;
            client& operator=(const client&) = delete;
        };
    }
}