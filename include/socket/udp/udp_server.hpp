/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/dgram_server.hpp>
#include <socket/inet/inet_address.hpp>
#include <socket/udp/udp_option.hpp>

namespace chen
{
    class udp_server : public dgram_server<inet_address, udp_option>
    {
    public:
        typedef dgram_server<inet_address, udp_option> super_class;

    public:
        /**
         * Construct by the port, use IPv6 by default
         * @attention modern os support both v4 & v6 traffic if you use AF_INET6 to bind socket
         */
        udp_server(std::uint16_t port, ip_address::Type type = ip_address::Type::IPv6);

        /**
         * Construct by the first resolved address
         * @attention throw exception if no dns record found or dns error
         */
        udp_server(const char *mixed, ip_address::Type type = ip_address::Type::None);
        udp_server(const std::string &mixed, ip_address::Type type = ip_address::Type::None);
        udp_server(const std::string &host, std::uint16_t port, ip_address::Type type = ip_address::Type::None);
        udp_server(const std::string &host, const std::string &service, ip_address::Type type = ip_address::Type::None);

        /**
         * Construct by the address
         */
        udp_server(const inet_address &addr);
    };
}