/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/inet/inet_endpoint.hpp>

namespace chen
{
    class inet_resolver
    {
    public:
        /**
         * Resolve host and service to endpoints
         * For Domain:
         * :-) resolve("chensoft.com")
         * :-) resolve("chensoft.com:80")
         * :-) resolve("chensoft.com:http")
         * For IPv4:
         * :-) resolve(":80")
         * :-) resolve("127.0.0.1")
         * :-) resolve("127.0.0.1:80")
         * :-) resolve("127.0.0.1:http")
         * For IPv6:
         * :-) resolve("[::]:80")
         * :-) resolve("[fe80::1]")
         * :-) resolve("[fe80::1]:80")
         * :-) resolve("[fe80::1]:http")
         * :-) resolve("[fe80::1%lo0]")
         * :-) resolve("[fe80::1%lo0]:80")
         * :-) resolve("[fe80::1%lo0]:http")
         */
        static std::vector<inet_endpoint> resolve(const std::string &mixed, ip_address::Type type = ip_address::Type::None);
        static std::vector<inet_endpoint> resolve(const std::string &host, std::uint16_t port, ip_address::Type type = ip_address::Type::None);
        static std::vector<inet_endpoint> resolve(const std::string &host, const std::string &service, ip_address::Type type = ip_address::Type::None);

        /**
         * Reverse resolve endpoint to host and service
         * make sure your ip address has PTR dns record
         * e.g: resolve 216.58.197.238 will search the PTR record of 238.197.58.216.in-addr.arpa
         */
        static std::pair<std::string, std::string> reverse(const inet_endpoint &ep);

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