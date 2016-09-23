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
        class endpoint
        {
        public:
            socklen_t size = 0;
            struct ::sockaddr_storage addr{};
        };
    }
}