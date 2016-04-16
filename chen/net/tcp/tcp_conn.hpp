/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "tcp_socket.hpp"

namespace chen
{
    namespace tcp
    {
        class conn : public socket
        {
        public:
            /**
             * Construct with a new socket
             * @param so socket after accept, int on Unix, SOCKET on Windows
             */
            explicit conn(void *so);

            virtual ~conn();
        };
    }
}