/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_version.hpp>

namespace chen
{
    namespace net
    {
        class address
        {
        public:
            enum class Type {None, IPv4, IPv6};

        public:
            /**
             * Construct an null address
             */
            address(std::nullptr_t);

            /**
             * Construct an empty address, v4 or v6
             */
            explicit address(Type type);

            /**
             * Construct by version directly
             */
            explicit address(version4 v4);
            explicit address(version6 v6);

            /**
             * Construct by ip address, accept the following format:
             * For IPv4:
             * :-) version4("127.0.0.1")
             * :-) version4("127.0.0.1/8")
             * :-) version4("127.0.0.1", 8)
             * :-) version4("127.0.0.1", "255.0.0.0")
             * :-) version4("127")       -> "127.0.0.0"
             * :-) version4("127.1")     -> "127.0.0.1"
             * :-) version4("192.168.1") -> "192.168.0.1"
             * For IPv6:
             * :-) version6("::")
             * :-) version6("::1")
             * :-) version6("0:0:0:0:0:0:0:1")
             * :-) version6("2404:6800:4004:817::200e")
             * :-) version6("2404:6800:4004:817:0:0:0:200e")
             * :-) version6("2404:6800:4004:817:0000:0000:0000:200e")
             * :-) version6("::192.168.0.1")
             * :-) version6("2404:6800:4004:817::200e/64")
             * :-) version6("2404:6800:4004:817::200e", 64)
             * :-) version6("2404:6800:4004:817::200e", "ffff:ffff:ffff:ffff::")
             * :-) version6("fe80::1%1")
             * :-) version6("fe80::1%lo0")
             * :-) version6("fe80::1%1/64")
             * :-) version6("fe80::1%lo0/64")
             * :-) version6("fe80::1%lo0", 64)
             * :-) version6("fe80::1%lo0", "ffff:ffff:ffff:ffff::")
             */
            address(const std::string &addr);
            address(const std::string &addr, std::uint8_t cidr);
            address(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
            address(const std::string &addr, const std::string &mask);
            address(const std::string &addr, const std::string &mask, std::uint32_t scope);

            address(const char *addr);
            address(const char *addr, std::uint8_t cidr);
            address(const char *addr, std::uint8_t cidr, std::uint32_t scope);
            address(const char *addr, const std::string &mask);
            address(const char *addr, const std::string &mask, std::uint32_t scope);

        public:
            /**
             * Assignment
             */
            void assign(Type type);

            void assign(version4 v4);
            void assign(version6 v6);

            void assign(const std::string &addr);
            void assign(const std::string &addr, std::uint8_t cidr);
            void assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
            void assign(const std::string &addr, const std::string &mask);
            void assign(const std::string &addr, const std::string &mask, std::uint32_t scope);

            address& operator=(Type type);
            address& operator=(version4 v4);
            address& operator=(version6 v6);
            address& operator=(const std::string &addr);
            address& operator=(const char *addr);

        public:
            /**
             * Detect type
             */
            Type type() const;

            bool isIPv4() const;
            bool isIPv6() const;

            const version4& v4() const;
            const version6& v6() const;

            version4& v4();
            version6& v6();

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

            std::uint8_t cidr() const;
            void cidr(std::uint8_t value);

            std::uint32_t scope() const;
            void scope(std::uint32_t value);

        public:
            /**
             * Network address based on current ip
             */
            address network() const;
            address minhost() const;
            address maxhost() const;

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
            bool operator==(const address &o) const;
            bool operator!=(const address &o) const;

            bool operator<(const address &o) const;
            bool operator>(const address &o) const;

            bool operator<=(const address &o) const;
            bool operator>=(const address &o) const;

            operator bool() const;

        public:
            /**
             * Common addresses
             */
            static address any(Type type);
            static address loopback(Type type);

        public:
            /**
             * Helper functions
             */
            static Type detect(const std::string &addr);

        private:
            Type _type;

            union
            {
                version4 v4;
                version6 v6;
            } _impl;
        };
    }
}