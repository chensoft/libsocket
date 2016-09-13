/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_address.hpp>
#include <socket/ip/ip_address.hpp>

namespace chen
{
    namespace net
    {
        /**
         * Endpoint for TCP & UDP
         */
        class address : public bsd::address
        {
        public:
            /**
             * Construct an null address
             */
            address(std::nullptr_t);

            /**
             * Construct by ip address & port
             */
            address(const ip::address &addr, std::uint16_t port);

            /**
             * Construct by "ip:port" string
             * service name will be converted to port number
             * For IPv4:
             * :-) address(":80")
             * :-) address("127.0.0.1")
             * :-) address("127.0.0.1:80")
             * :-) address("127.0.0.1:http")
             * For IPv6
             * :-) address("[::]:80")
             * :-) address("[fe80::1]")
             * :-) address("[fe80::1]:80")
             * :-) address("[fe80::1]:http")
             * :-) address("[fe80::1%lo0]")
             * :-) address("[fe80::1%lo0]:80")
             * :-) address("[fe80::1%lo0]:http")
             * todo allow domain here, use first entry
             * @notice don't use domain here, accept ip address only
             */
            address(const std::string &mixed);
            address(const char *mixed);

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
             * Comparison
             */
            bool operator==(const address &o) const;
            bool operator!=(const address &o) const;

            bool operator<(const address &o) const;
            bool operator>(const address &o) const;

            bool operator<=(const address &o) const;
            bool operator>=(const address &o) const;

        public:
            /**
             * Override
             */
            virtual struct ::sockaddr_storage get() const override;
            virtual void set(const struct ::sockaddr_storage &val) override;
            virtual socklen_t len() const override;

        private:
            ip::address _addr;
            std::uint16_t _port = 0;
        };
    }
}