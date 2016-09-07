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
        class server : public basic
        {
        public:
            /**
             * Start the server
             * todo create notifier in server, spawn tcp::conn with notifier, shared_ptr<notifier>
             */
            chen::status start(std::uint16_t port);
            chen::status start(const endpoint &ep);
            chen::status start(const ip::address &addr, std::uint16_t port);

            /**
             * Stop the server
             * todo how to exit, using pipe?
             */
            chen::status stop();
        };
    }
}