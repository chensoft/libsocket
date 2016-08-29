/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_address.hpp>

namespace chen
{
    namespace net
    {
        class endpoint
        {
        public:
            /**
             * Construct an null address
             */
            endpoint(std::nullptr_t);

            /**
             * Construct by ip address and port
             */
            endpoint(const address &addr, std::uint16_t port);

            /**
             * Construct by ip <-> port pair
             */
            endpoint(std::pair<std::string, std::uint16_t> pair);

            /**
             * Construct by "ip:port" string
             * service name will be converted to port number
             * For IPv4:
             * :-) endpoint(":80")
             * :-) endpoint("127.0.0.1")
             * :-) endpoint("127.0.0.1:80")
             * :-) endpoint("127.0.0.1:http")
             * For IPv6
             * :-) endpoint("[::]:80")
             * :-) endpoint("[fe80::1]")
             * :-) endpoint("[fe80::1]:80")
             * :-) endpoint("[fe80::1]:http")
             * :-) endpoint("[fe80::1%lo0]")
             * :-) endpoint("[fe80::1%lo0]:80")
             * :-) endpoint("[fe80::1%lo0]:http")
             * @caution don't use domain in here, accept ip address only
             */
            endpoint(const std::string &mixed);

        public:
            /**
             * Standard representation, with CIDR or not
             * e.g: 127.0.0.1:80
             * e.g: 127.0.0.1/8:80
             * e.g: [2404:6800:4004:817::200e]:80
             * e.g: [2404:6800:4004:817::200e/64]:80
             */
            std::string str(bool cidr = false, bool scope = false) const;

            /**
             * Property
             */
            bool empty() const;

            const address& addr() const;
            void addr(const address &value);

            std::uint16_t port() const;
            void port(std::uint16_t value);

            /**
             * Special ports
             * @link http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
             */
            bool isWellKnownPort() const;
            bool isRegisteredPort() const;
            bool isDynamicPort() const;

        public:
            /**
             * Operator
             */
            bool operator==(const endpoint &o) const;
            bool operator!=(const endpoint &o) const;

            bool operator<(const endpoint &o) const;
            bool operator>(const endpoint &o) const;

            bool operator<=(const endpoint &o) const;
            bool operator>=(const endpoint &o) const;

            operator bool() const;

        public:
            /**
             * Conversion
             */
            static endpoint toEndpoint(const struct sockaddr *addr);
            static endpoint toEndpoint(const struct sockaddr_in *addr);
            static endpoint toEndpoint(const struct sockaddr_in6 *addr);
            static endpoint toEndpoint(const struct sockaddr_storage *addr);

            static void toAddress(const endpoint &ep, struct sockaddr_storage &out, socklen_t &len);

        private:
            address _addr;
            std::uint16_t _port = 0;
        };
    }
}