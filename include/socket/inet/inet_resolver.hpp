/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/inet/inet_address.hpp>

namespace chen
{
    class inet_resolver
    {
    public:
        /**
         * Resolve host and service to addresses
         * For Domain:
         * :-) inet_resolve("chensoft.com")
         * :-) inet_resolve("chensoft.com:80")
         * :-) inet_resolve("chensoft.com:http")
         * For IPv4:
         * :-) inet_resolve(":80")
         * :-) inet_resolve("127.0.0.1")
         * :-) inet_resolve("127.0.0.1:80")
         * :-) inet_resolve("127.0.0.1:http")
         * For IPv6:
         * :-) inet_resolve("[::]:80")
         * :-) inet_resolve("[fe80::1]")
         * :-) inet_resolve("[fe80::1]:80")
         * :-) inet_resolve("[fe80::1]:http")
         * :-) inet_resolve("[fe80::1%lo0]")
         * :-) inet_resolve("[fe80::1%lo0]:80")
         * :-) inet_resolve("[fe80::1%lo0]:http")
         */
        static std::vector<inet_address> resolve(const std::string &mixed, ip_address::Type type = ip_address::Type::None);

        /**
         * Reverse resolve address to host and service
         * make sure your ip address has PTR dns record
         * e.g: resolve 216.58.197.238 will search the PTR record of 238.197.58.216.in-addr.arpa
         */
        static std::pair<std::string, std::string> reverse(const inet_address &addr);

        /**
         * Resolve service
         */
        static std::uint16_t service(const std::string &name, const std::string &protocol = "");
        static std::string service(std::uint16_t port, const std::string &protocol = "");

        /**
         * Extract host & service
         */
        static std::pair<std::string, std::string> extract(const std::string &mixed);
    };
}