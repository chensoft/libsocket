/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/udp/udp_basic.hpp>

namespace chen
{
    namespace udp
    {
        class server : public basic
        {
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