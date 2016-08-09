/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "ip_subnet.hpp"

namespace chen
{
    namespace ip
    {
        class interface
        {
        public:
            /**
             * Enumerate all interfaces
             */
            static std::vector<interface> enumerate();

        public:
            std::string name;               // interface name, e.g: lo0, en0, eth0
            std::uint32_t flag = 0;         // interface flags, determine status, broadcast and more
            std::shared_ptr<address> addr;  // ip address, its real type is subnet_v4 or subnet_v6
        };
    }
}