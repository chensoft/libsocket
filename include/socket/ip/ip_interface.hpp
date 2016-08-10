/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "ip_address.hpp"
#include <map>

namespace chen
{
    namespace ip
    {
        class interface
        {
        public:
            typedef std::shared_ptr<address> address;

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
            static std::map<std::string, interface> enumerate();

            /**
             * Get IPv6 scope id by address and interface name
             * if the name is a integer, then it's the scope zone id
             */
            static std::uint32_t scope(const std::array<std::uint8_t, 16> &addr, const std::string &name);

        public:
            std::string name;           // interface name, e.g: lo0, en0, eth0
            std::uint32_t flag = 0;     // interface flags, determine status, broadcast and more
            std::vector<address> addr;  // all ip addresses on this interface, each address has netmask, broadcast and scope id
        };
    }
}