/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_option.hpp>

namespace chen
{
    namespace tcp
    {
        class option : public ip::option
        {
        public:
            option(socket_t fd);

        public:
            /**
             * TCP_NODELAY(turn off the TCP Nagle algorithm)
             */
            bool nodelay() const;
            bool nodelay(bool val);
        };
    }
}