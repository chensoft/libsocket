/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_endpoint.hpp>
#include <socket/ip/ip_address.hpp>

namespace chen
{
    namespace net
    {
        /**
         * Endpoint for TCP & UDP
         */
        class endpoint
        {
        public:
            /**
             * Construct an null address
             */
            endpoint(std::nullptr_t = nullptr);

            /**
             * Construct from raw bsd endpoint
             */
            endpoint(const bsd::endpoint &ep);
            endpoint(const struct ::sockaddr *ptr);

            /**
             * Construct by ip address & port
             * todo move to below constructor
             */
            endpoint(const ip::address &addr, std::uint16_t port);

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
             * todo allow domain here, use first entry
             * @notice don't use domain here, accept ip address only
             */
            endpoint(const std::string &mixed);
            endpoint(const char *mixed);

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
            operator bool() const;

            const ip::address& addr() const;
            void addr(const ip::address &value);

            std::uint16_t port() const;
            void port(std::uint16_t value);

            bsd::endpoint raw() const;

            /**
             * Special ports
             * @link http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
             */
            bool isWellKnownPort() const;
            bool isRegisteredPort() const;
            bool isDynamicPort() const;

        public:
            /**
             * Comparison
             */
            bool operator==(const endpoint &o) const;
            bool operator!=(const endpoint &o) const;

            bool operator<(const endpoint &o) const;
            bool operator>(const endpoint &o) const;

            bool operator<=(const endpoint &o) const;
            bool operator>=(const endpoint &o) const;

        private:
            ip::address _addr;
            std::uint16_t _port = 0;
        };
    }
}