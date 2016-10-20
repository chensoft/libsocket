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
             * Construct an null endpoint
             */
            endpoint(std::nullptr_t = nullptr);

            /**
             * Construct by endpoint representation
             * For IPv4:
             * :-) endpoint(":80")
             * :-) endpoint("127.0.0.1")
             * :-) endpoint("127.0.0.1:80")
             * :-) endpoint("127.0.0.1:http")
             * For IPv6:
             * :-) endpoint("[::]:80")
             * :-) endpoint("[fe80::1]")
             * :-) endpoint("[fe80::1]:80")
             * :-) endpoint("[fe80::1]:http")
             * :-) endpoint("[fe80::1%lo0]")
             * :-) endpoint("[fe80::1%lo0]:80")
             * :-) endpoint("[fe80::1%lo0]:http")
             */
            endpoint(const std::string &mixed);

            /**
             * Construct by ip address and port or service
             */
            endpoint(const ip::address &addr, std::uint16_t port);
            endpoint(const ip::address &addr, const std::string &service);

            /**
             * Construct by raw bsd endpoint
             */
            endpoint(const bsd::endpoint &ep);
            endpoint(const struct ::sockaddr *ep);

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

            /**
             * Special ports
             * @link http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
             */
            bool isWellKnownPort() const;
            bool isRegisteredPort() const;
            bool isDynamicPort() const;

        public:
            /**
             * Assignment
             */
            endpoint& operator=(std::nullptr_t);
            endpoint& operator=(const std::string &mixed);
            endpoint& operator=(const bsd::endpoint &ep);
            endpoint& operator=(const struct ::sockaddr *ep);

            /**
             * Conversion
             */
            operator chen::bsd::endpoint() const;

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