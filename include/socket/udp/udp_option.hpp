/**
 * Created by Jian Chen
 * @since  2016.11.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_option.hpp>

namespace chen
{
    namespace udp
    {
        class option : public ip_option
        {
        public:
            option(basic_socket &s);
        };
    }
}