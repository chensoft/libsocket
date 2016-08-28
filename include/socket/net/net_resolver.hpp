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
             * todo add family param or automatically peek tcp, udp
             */
            static std::vector<endpoint> resolve(const std::string &mixed);
            static std::vector<endpoint> resolve(const std::string &host, std::uint16_t port);
            static std::vector<endpoint> resolve(const std::string &host, const std::string &service);

        public:
            /**
             * Resolve address
             * todo determine more hints param
             */
            static std::vector<net::address> address(const std::string &host);
            static std::vector<net::address> address(const std::string &host, int family);

            /**
             * Resolve service
             * todo determine more hints param
             */
            static std::uint16_t service(const std::string &name, const std::string &protocol = "");

        public:
            /**
             * Split host and port into a pair
             * For Domain:
             * :-) split("chensoft.com")
             * :-) split("chensoft.com:80")
             * :-) split("chensoft.com:http")
             * For IPv4:
             * :-) split(":80")
             * :-) split("127.0.0.1")
             * :-) split("127.0.0.1:80")
             * :-) split("127.0.0.1:http")
             * For IPv6:
             * :-) endpoint("[::]:80")
             * :-) endpoint("[fe80::1]")
             * :-) endpoint("[fe80::1]:80")
             * :-) endpoint("[fe80::1]:http")
             * :-) endpoint("[fe80::1%lo0]")
             * :-) endpoint("[fe80::1%lo0]:80")
             * :-) endpoint("[fe80::1%lo0]:http")
             */
            static std::pair<std::string, std::uint16_t> split(const std::string &mixed);
        };
    }
}