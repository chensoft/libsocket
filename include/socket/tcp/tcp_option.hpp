/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_option.hpp>

namespace chen
{
    namespace tcp
    {
        namespace option
        {
            // -----------------------------------------------------------------
            // TCP_NODELAY(turn off the TCP Nagle algorithm)
            class nodelay : public chen::option::basic
            {
            public:
                explicit nodelay(const socket &sock);
                explicit nodelay(bool val);

            public:
                bool apply(socket &sock);

            public:
                bool enable = false;
            };
        }
    }
}