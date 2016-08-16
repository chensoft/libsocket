/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_address.hpp>
#include <map>

namespace chen
{
    namespace net
    {
        class interface
        {
        public:
            /**
             * Flags
             */
            bool isUp()          const;
            bool isBroadcast()   const;
            bool isLoopback()    const;
            bool isRunning()     const;
            bool isPromiscuous() const;
            bool isMulticast()   const;

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

            /**
             * Get interface name by IPv6 scope id
             * if interface is not found then return the integer string
             */
            static std::string scope(std::uint32_t id);

        public:
            std::string name;           // interface name, e.g: lo0, en0, eth0
            std::uint32_t flag = 0;     // interface flags, determine status, broadcast and more
            std::vector<address> addr;  // all ip addresses on this interface, each address has netmask, broadcast and scope id
        };
    }
}