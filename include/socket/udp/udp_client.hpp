/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/dgram_client.hpp>
#include <socket/inet/inet_address.hpp>
#include <socket/udp/udp_option.hpp>

namespace chen
{
    class udp_client : public dgram_client<inet_address, udp_option>
    {
    public:
        typedef dgram_client<inet_address, udp_option> super_class;

    public:
        udp_client(int family);
    };
}