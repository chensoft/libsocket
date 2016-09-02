/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_conn.hpp>

namespace chen
{
    namespace tcp
    {
        class server : public basic
        {
        public:
            /**
             * Start the server
             */
            bool start(const endpoint &ep);
            bool start(const ip::address &addr, std::uint16_t port);

            /**
             * Stop the server
             */
            void stop();

        private:
//            bool _exit = false;  // todo
        };
    }
}