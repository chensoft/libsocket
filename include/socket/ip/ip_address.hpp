/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_version.hpp>
#include <socket/config.hpp>

namespace chen
{
    class ip_address
    {
    public:
        enum class Type : std::uint8_t
        {
            None = AF_UNSPEC,
            IPv4 = AF_INET,
            IPv6 = AF_INET6
        };

    public:
        /**
         * Construct an null address
         */
        ip_address(std::nullptr_t = nullptr);

        /**
         * Construct an empty address, v4 or v6
         */
        explicit ip_address(Type type);

        /**
         * Construct by version directly
         */
        ip_address(const ip_version4 &v4);
        ip_address(const ip_version6 &v6);

        /**
         * Construct by ip address, accept the following format:
         * For IPv4:
         * :-) ip_address("127.0.0.1")
         * :-) ip_address("127.0.0.1/8")
         * :-) ip_address("127.0.0.1", 8)
         * :-) ip_address("127.0.0.1", "255.0.0.0")
         * :-) ip_address("127")       -> "127.0.0.0"
         * :-) ip_address("127.1")     -> "127.0.0.1"
         * :-) ip_address("192.168.1") -> "192.168.0.1"
         * For IPv6:
         * :-) ip_address("::")
         * :-) ip_address("::1")
         * :-) ip_address("0:0:0:0:0:0:0:1")
         * :-) ip_address("2404:6800:4004:817::200e")
         * :-) ip_address("2404:6800:4004:817:0:0:0:200e")
         * :-) ip_address("2404:6800:4004:817:0000:0000:0000:200e")
         * :-) ip_address("::192.168.0.1")
         * :-) ip_address("2404:6800:4004:817::200e/64")
         * :-) ip_address("2404:6800:4004:817::200e", 64)
         * :-) ip_address("2404:6800:4004:817::200e", "ffff:ffff:ffff:ffff::")
         * :-) ip_address("fe80::1%1")
         * :-) ip_address("fe80::1%lo0")
         * :-) ip_address("fe80::1%1/64")
         * :-) ip_address("fe80::1%lo0/64")
         * :-) ip_address("fe80::1%lo0", 64)
         * :-) ip_address("fe80::1%lo0", "ffff:ffff:ffff:ffff::")
         */
        ip_address(const char *addr);

        ip_address(const std::string &addr);
        ip_address(const std::string &addr, std::uint8_t cidr);
        ip_address(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
        ip_address(const std::string &addr, const std::string &mask);
        ip_address(const std::string &addr, const std::string &mask, std::uint32_t scope);

    public:
        /**
         * Assignment
         */
        void assign(std::nullptr_t);
        void assign(Type type);

        void assign(const ip_version4 &v4);
        void assign(const ip_version6 &v6);

        void assign(const std::string &addr);
        void assign(const std::string &addr, std::uint8_t cidr);
        void assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
        void assign(const std::string &addr, const std::string &mask);
        void assign(const std::string &addr, const std::string &mask, std::uint32_t scope);

        ip_address& operator=(std::nullptr_t);
        ip_address& operator=(Type type);
        ip_address& operator=(const ip_version4 &v4);
        ip_address& operator=(const ip_version6 &v6);
        ip_address& operator=(const char *addr);
        ip_address& operator=(const std::string &addr);

    public:
        /**
         * Detect type
         */
        Type type() const;

        bool isIPv4() const;
        bool isIPv6() const;

        const ip_version4& v4() const;
        const ip_version6& v6() const;

        ip_version4& v4();
        ip_version6& v6();

    public:
        /**
         * Standard representation, with CIDR or not
         * e.g: 127.0.0.1
         * e.g: 127.0.0.1/8
         * e.g: 2404:6800:4004:817::200e
         * e.g: 2404:6800:4004:817::200e/64
         */
        std::string str(bool cidr = false, bool scope = false) const;

        /**
         * Binary representation
         */
        std::vector<std::uint8_t> bytes() const;

        /**
         * Get raw value
         */
        bool empty() const;
        operator bool() const;

        std::uint8_t cidr() const;
        void cidr(std::uint8_t value);

        std::uint32_t scope() const;
        void scope(std::uint32_t value);

    public:
        /**
         * Network address based on current ip
         */
        ip_address network() const;
        ip_address minhost() const;
        ip_address maxhost() const;

    public:
        /**
         * Special addressing type
         */
        bool isUnspecified() const;
        bool isLoopback() const;
        bool isMulticast() const;

    public:
        /**
         * Operator, consider CIDR and scope
         */
        bool operator==(const ip_address &o) const;
        bool operator!=(const ip_address &o) const;

        bool operator<(const ip_address &o) const;
        bool operator>(const ip_address &o) const;

        bool operator<=(const ip_address &o) const;
        bool operator>=(const ip_address &o) const;

    public:
        /**
         * Common addresses
         */
        static ip_address any(Type type);
        static ip_address loopback(Type type);

    public:
        /**
         * Helper functions
         */
        static Type detect(const std::string &addr);

    private:
        Type _type;

        union
        {
            ip_version4 v4;
            ip_version6 v6;
        } _impl;
    };
}