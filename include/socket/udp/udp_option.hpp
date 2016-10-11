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
    namespace udp
    {
        class option : public ip::option
        {
        public:
            option(bsd::socket &s);
        };
    }
}