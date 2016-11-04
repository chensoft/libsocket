/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/basic_endpoint.hpp>
#include <socket/net/ip/ip_address.hpp>

namespace chen
{
    /**
     * Endpoint for TCP & UDP
     */
    class inet_endpoint
    {
    public:
        /**
         * Construct an null endpoint
         */
        inet_endpoint(std::nullptr_t = nullptr);

        /**
         * Construct by endpoint representation
         * For IPv4:
         * :-) inet_endpoint(":80")
         * :-) inet_endpoint("127.0.0.1")
         * :-) inet_endpoint("127.0.0.1:80")
         * :-) inet_endpoint("127.0.0.1:http")
         * For IPv6:
         * :-) inet_endpoint("[::]:80")
         * :-) inet_endpoint("[fe80::1]")
         * :-) inet_endpoint("[fe80::1]:80")
         * :-) inet_endpoint("[fe80::1]:http")
         * :-) inet_endpoint("[fe80::1%lo0]")
         * :-) inet_endpoint("[fe80::1%lo0]:80")
         * :-) inet_endpoint("[fe80::1%lo0]:http")
         */
        inet_endpoint(const char *mixed);
        inet_endpoint(const std::string &mixed);

        /**
         * Construct by ip address and port or service
         */
        inet_endpoint(const ip::address &addr, std::uint16_t port);
        inet_endpoint(const ip::address &addr, const std::string &service);

        /**
         * Construct by raw bsd endpoint
         */
        inet_endpoint(const basic_endpoint &ep);
        inet_endpoint(const basic_endpoint &ep, std::uint16_t port);
        inet_endpoint(const basic_endpoint &ep, const std::string &service);

        inet_endpoint(const struct ::sockaddr *ep);
        inet_endpoint(const struct ::sockaddr *ep, std::uint16_t port);
        inet_endpoint(const struct ::sockaddr *ep, const std::string &service);

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
        void assign(std::nullptr_t);
        void assign(const std::string &mixed);

        void assign(const ip::address &addr, std::uint16_t port);
        void assign(const ip::address &addr, const std::string &service);

        void assign(const basic_endpoint &ep);
        void assign(const basic_endpoint &ep, std::uint16_t port);
        void assign(const basic_endpoint &ep, const std::string &service);

        void assign(const struct ::sockaddr *ep);
        void assign(const struct ::sockaddr *ep, std::uint16_t port);
        void assign(const struct ::sockaddr *ep, const std::string &service);

        inet_endpoint& operator=(std::nullptr_t);
        inet_endpoint& operator=(const char *mixed);
        inet_endpoint& operator=(const std::string &mixed);
        inet_endpoint& operator=(const basic_endpoint &ep);
        inet_endpoint& operator=(const struct ::sockaddr *ep);

        /**
         * Conversion
         */
        operator chen::basic_endpoint() const;

        /**
         * Comparison
         */
        bool operator==(const inet_endpoint &o) const;
        bool operator!=(const inet_endpoint &o) const;

        bool operator<(const inet_endpoint &o) const;
        bool operator>(const inet_endpoint &o) const;

        bool operator<=(const inet_endpoint &o) const;
        bool operator>=(const inet_endpoint &o) const;

    private:
        ip::address _addr;
        std::uint16_t _port = 0;
    };
}