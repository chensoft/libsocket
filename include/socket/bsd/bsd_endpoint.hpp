/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>

namespace chen
{
    namespace bsd
    {
        /**
         * bsd socket address wrapper, you don't need to use it directly
         */
        class endpoint
        {
        public:
            socklen_t size = 0;
            struct ::sockaddr_storage addr{};
        };
    }
}