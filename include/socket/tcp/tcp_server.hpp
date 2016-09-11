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
            std::error_code start(std::uint16_t port);
            std::error_code start(const endpoint &ep);
            std::error_code start(const ip::address &addr, std::uint16_t port);

            /**
             * Stop the server
             * todo how to exit, using pipe?
             */
            std::error_code stop();
        };
    }
}