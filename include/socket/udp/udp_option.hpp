/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_option.hpp>

namespace chen
{
    class udp_option : public ip_option
    {
    public:
        udp_option(bsd_socket &s) : ip_option(s)
        {
        }
    };
}