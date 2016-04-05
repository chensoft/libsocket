/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "tcp_socket.hpp"

namespace chen
{
    namespace tcp
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