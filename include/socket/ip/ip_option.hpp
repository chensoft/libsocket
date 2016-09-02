/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_option.hpp>

namespace chen
{
    namespace ip
    {
        namespace option
        {
            // -----------------------------------------------------------------
            // IP_TTL(IP packet's ttl value)
            class ttl : public net::option::basic
            {
            public:
                explicit ttl(const net::socket &sock);
                explicit ttl(int val);

            public:
                bool apply(net::socket &sock);

            public:
                int val = 0;
            };
        }
    }
}