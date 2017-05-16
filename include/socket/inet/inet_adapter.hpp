/**
 * Created by Jian Chen
 * @since  2016.08.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "socket/ip/ip_address.hpp"
#include <map>

namespace chen
{
    class inet_adapter
    {
    public:
        /**
         * Enumerate all network interfaces
         * @note only enumerate interfaces with IPv4 or IPv6 address
         */
        static std::map<std::string, inet_adapter> enumerate();

        /**
         * Get IPv6 scope id by address and interface name
         * if the name is a integer, then it's the scope zone id
         */
        static std::uint32_t scope(const std::uint8_t addr[16], const std::string &name);

        /**
         * Get interface name by IPv6 scope id
         * if interface is not found then return the integer string
         */
        static std::string scope(std::uint32_t id);

    public:
        std::string name;              // interface name, e.g: lo0, en0, eth0
        std::string mac;               // interface mac address, e.g: 78:31:c1:c2:b5:aa
        std::uint32_t  mtu = 0;        // interface mtu value, e.g: 1500
        std::uint32_t flag = 0;        // interface flags, determine status, broadcast and more, it's platform-dependent
        std::vector<ip_address> addr;  // all ip addresses on this interface, each address has netmask, broadcast and scope id
    };
}