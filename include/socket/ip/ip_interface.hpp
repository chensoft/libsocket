/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "ip_address.hpp"

namespace chen
{
    namespace ip
    {
        class interface
        {
        public:
            /**
             * Flags
             */
            bool isUp() const;
            bool isBroadcast() const;
            bool isLoopback() const;
            bool isRunning() const;
            bool isPromiscuous() const;
            bool isMulticast() const;

        public:
            /**
             * Enumerate all interfaces
             * @caution only enumerate interfaces with IPv4 or IPv6 address
             */
            static std::vector<interface> enumerate();

        public:
            std::string name;               // interface name, e.g: lo0, en0, eth0
            std::uint32_t flag = 0;         // interface flags, determine status, broadcast and more
            std::shared_ptr<address> addr;  // ip address, netmask, broadcast all in one
        };
    }
}