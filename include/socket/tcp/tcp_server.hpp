/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_basic.hpp>

namespace chen
{
    namespace tcp
    {
        // todo add notify
        class server : public basic
        {
        public:
            /**
             * Start the server
             */
            void start(const net::endpoint &ep);
            void start(const net::address &addr, std::uint16_t port);

            /**
             * Stop the server
             */
            void stop();
        };
    }
}