/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_address.hpp>
#include <socket/ip/ip_address.hpp>

namespace chen
{
    /**
     * Address for TCP & UDP
     */
    class inet_address
    {
    public:
        /**
         * Construct an null address
         */
        inet_address(std::nullptr_t = nullptr);

        /**
         * Construct by address representation
         * For IPv4:
         * :-) inet_address(":80")
         * :-) inet_address("127.0.0.1")
         * :-) inet_address("127.0.0.1:80")
         * :-) inet_address("127.0.0.1:http")
         * For IPv6(no port):
         * :-) inet_resolve("fe80::1")
         * :-) inet_resolve("fe80::1%lo0")
         * :-) inet_resolve("[fe80::1]")
         * :-) inet_resolve("[fe80::1%lo0]")
         * For IPv6(with port):
         * :-) inet_resolve("[::]:80")
         * :-) inet_resolve("[fe80::1]:80")
         * :-) inet_resolve("[fe80::1]:http")
         * :-) inet_resolve("[fe80::1%lo0]:80")
         * :-) inet_resolve("[fe80::1%lo0]:http")
         */
        inet_address(const char *mixed);
        inet_address(const std::string &mixed);

        /**
         * Construct by ip address and port or service
         */
        inet_address(const ip_address &addr, std::uint16_t port);
        inet_address(const ip_address &addr, const std::string &service);

        /**
         * Construct by raw bsd address
         */
        inet_address(const basic_address &addr);
        inet_address(const basic_address &addr, std::uint16_t port);
        inet_address(const basic_address &addr, const std::string &service);

        inet_address(const struct ::sockaddr *addr);
        inet_address(const struct ::sockaddr *addr, std::uint16_t port);
        inet_address(const struct ::sockaddr *addr, const std::string &service);

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

        const ip_address& addr() const;
        void addr(const ip_address &value);

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

        void assign(const ip_address &addr, std::uint16_t port);
        void assign(const ip_address &addr, const std::string &service);

        void assign(const basic_address &addr);
        void assign(const basic_address &addr, std::uint16_t port);
        void assign(const basic_address &addr, const std::string &service);

        void assign(const struct ::sockaddr *addr);
        void assign(const struct ::sockaddr *addr, std::uint16_t port);
        void assign(const struct ::sockaddr *addr, const std::string &service);

        inet_address& operator=(std::nullptr_t);
        inet_address& operator=(const char *mixed);
        inet_address& operator=(const std::string &mixed);
        inet_address& operator=(const basic_address &addr);
        inet_address& operator=(const struct ::sockaddr *addr);

        /**
         * Conversion
         */
        operator chen::basic_address() const;

        /**
         * Comparison
         */
        bool operator==(const inet_address &o) const;
        bool operator!=(const inet_address &o) const;

        bool operator<(const inet_address &o) const;
        bool operator>(const inet_address &o) const;

        bool operator<=(const inet_address &o) const;
        bool operator>=(const inet_address &o) const;

    private:
        ip_address _addr;
        std::uint16_t _port = 0;
    };
}