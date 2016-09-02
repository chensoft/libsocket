/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_common.hpp>

namespace chen
{
    namespace udp
    {
        class server : public common
        {
            /**
             * Start the server
             */
            void start(const endpoint &ep);
            void start(const ip::address &addr, std::uint16_t port);

            /**
             * Stop the server
             */
            void stop();
        };
    }
}