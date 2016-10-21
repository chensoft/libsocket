/**
 * Created by Jian Chen
 * @since  2016.09.21
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
            static std::vector<net::endpoint> resolve(const std::string &mixed);
            static std::vector<net::endpoint> resolve(const std::string &mixed, ip::address::Type type);

            static std::vector<net::endpoint> resolve(const std::string &host, std::uint16_t port);
            static std::vector<net::endpoint> resolve(const std::string &host, std::uint16_t port, ip::address::Type type);

            static std::vector<net::endpoint> resolve(const std::string &host, const std::string &service);
            static std::vector<net::endpoint> resolve(const std::string &host, const std::string &service, ip::address::Type type);

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
}