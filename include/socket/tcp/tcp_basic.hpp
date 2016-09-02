/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>

namespace chen
{
    namespace tcp
    {
        class basic
        {
        public:
            /**
             * Local and remote endpoint
             */
            net::endpoint local() const;
            net::endpoint remote() const;

            /**
             * Non-blocking status
             */
            bool nonblocking() const;
            bool nonblocking(bool enable);

            /**
             * Check socket is empty
             */
            bool empty() const;
            operator bool() const;

            /**
             * Native socket handle
             */
            socket_t native() const;

        protected:
            net::socket _socket;
        };
    }
}