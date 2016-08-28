/**
 * Created by Jian Chen
 * @since  2016.08.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>

namespace chen
{
    namespace net
    {
        class resolver
        {
        public:
            /**
             * Resolve host and service, return endpoints
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
            static std::vector<endpoint> resolve(const std::string &mixed);
            static std::vector<endpoint> resolve(const std::string &mixed, address::Type type);

            static std::vector<endpoint> resolve(const std::string &host, const std::string &service);
            static std::vector<endpoint> resolve(const std::string &host, const std::string &service, address::Type type);

            static std::vector<endpoint> resolve(const std::string &host, std::uint16_t port);
            static std::vector<endpoint> resolve(const std::string &host, std::uint16_t port, address::Type type);

        public:
            /**
             * Resolve address
             */
            static std::vector<net::address> address(const std::string &host);
            static std::vector<net::address> address(const std::string &host, net::address::Type type);

            /**
             * Resolve service
             */
            static std::uint16_t service(const std::string &name, const std::string &protocol = "");
            static std::string service(std::uint16_t port, const std::string &protocol = "");

        public:
            /**
             * Split host and port into a pair
             */
            static std::pair<std::string, std::uint16_t> split(const std::string &mixed);
        };
    }
}