/**
 * Created by Jian Chen
 * @since  2016.07.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

namespace chen
{
    namespace ip
    {
        class address
        {
        public:
            virtual ~address() = default;

        public:
            /**
             * Simple string representation
             */
            virtual std::string str() const = 0;

            /**
             * Full string representation
             */
            virtual std::string full() const = 0;

            /**
             * Get encoded bytes
             */
            virtual std::vector<std::uint8_t> bytes() const = 0;
        };

        class address_v4 : public address
        {
        public:
            address_v4() = default;

            /**
             * Construct by ipv4 address and CIDR prefix
             * @e.g: address_v4("127.0.0.1") or address_v4("127.0.0.1/8")
             * @e.g: address_v4("127.0.0.1", 8)
             * @e.g: address_v4("127.0.0.1", "255.0.0.0")
             * Accept some special address format
             * @e.g: "127" -> "127.0.0.0"
             * @e.g: "127.1" -> "127.0.0.1"
             * @e.g: "192.168.1" -> "192.168.0.1"
             */
            address_v4(const std::string &addr);
            address_v4(const std::string &addr, std::uint8_t cidr);
            address_v4(const std::string &addr, const std::string &mask);

            /**
             * Construct by ipv4 integer and CIDR prefix
             * @e.g: "127.0.0.0" is 0x7F000000, also is 2130706433
             * @e.g: address_v4(2130706433)
             * @e.g: address_v4(2130706433, 8)
             * @e.g: address_v4(2130706433, "255.0.0.0")
             */
            address_v4(std::uint32_t addr);
            address_v4(std::uint32_t addr, std::uint8_t cidr);
            address_v4(std::uint32_t addr, const std::string &mask);

        public:
            /**
             * Simple string representation
             * @e.g: "127.0.0.1"
             */
            virtual std::string str() const override;

            /**
             * Full string representation with CIDR prefix
             * @e.g: "127.0.0.1/8"
             */
            virtual std::string full() const override;

            /**
             * Get encoded bytes by network order, length is 4 bytes
             */
            virtual std::vector<std::uint8_t> bytes() const override;

            /**
             * Get raw value
             */
            std::uint32_t addr() const;
            std::uint8_t  cidr() const;

            /**
             * Netmask & Wildcard mask
             */
            std::uint32_t netmask() const;
            std::uint32_t wildcard() const;

            /**
             * Network address based on current ip
             */
            address_v4 network() const;
            address_v4 minhost() const;
            address_v4 maxhost() const;
            address_v4 broadcast() const;

            /**
             * Calculate hosts count based on this network
             */
            std::size_t hosts() const;

        public:
            /**
             * Special addressing type
             */
            bool isReserved() const;
            bool isPrivate() const;
            bool isLoopback() const;
            bool isLinkLocal() const;
            bool isMulticast() const;
            bool isBroadcast() const;

        public:
            /**
             * Classful addressing type
             * @caution since CIDR is widely used today, classful is not very useful now
             */
            bool isClassA() const;
            bool isClassB() const;
            bool isClassC() const;
            bool isClassD() const;
            bool isClassE() const;

        public:
            /**
             * Operator, compare based on ipv4 address
             * @caution compare will ignore CIDR prefix
             */
            bool operator==(const address_v4 &o) const;
            bool operator!=(const address_v4 &o) const;

            bool operator<(const address_v4 &o) const;
            bool operator>(const address_v4 &o) const;

            bool operator<=(const address_v4 &o) const;
            bool operator>=(const address_v4 &o) const;

        public:
            /**
             * Convert between integer and string
             */
            static std::string toString(std::uint32_t addr);
            static std::string toString(std::uint32_t addr, std::uint8_t cidr);

            static std::uint32_t toInteger(const std::string &addr);
            static std::uint32_t toInteger(const std::string &addr, std::uint8_t &cidr);

        protected:
            std::uint32_t _addr = 0;  // 32 bit ipv4 address
            std::uint8_t  _cidr = 0;  // CIDR notation prefix length
        };

        class address_v6 : public address
        {

        };

        // helper
        template <class _CharT, class _Traits>
        std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& os, const address& addr)
        {
            // simple string representation
            return os << addr.str();
        }
    }
}