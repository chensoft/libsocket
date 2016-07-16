/**
 * Created by Jian Chen
 * @since  2016.07.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <stdexcept>
#include <cstdint>
#include <string>
#include <memory>
#include <array>

namespace chen
{
    namespace ip
    {
        class address
        {
        public:
            class error : public std::runtime_error
            {
            public:
                explicit error(const std::string &what) : std::runtime_error(what) {}
            };

        public:
            virtual ~address() = default;

            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const = 0;

        public:
            /**
             * Standard canonical representation
             */
            virtual std::string str() const = 0;

        public:
            /**
             * Generate ipv4 or ipv6 address automatically
             */
            static std::shared_ptr<chen::ip::address> create(const std::string &addr);
            static std::shared_ptr<chen::ip::address> create(const std::string &addr, std::uint8_t cidr);

        public:
            /**
             * Operator
             */
            virtual bool operator==(const address &o) const = 0;
            virtual bool operator!=(const address &o) const;

            virtual bool operator<(const address &o) const = 0;
            virtual bool operator>(const address &o) const;

            virtual bool operator<=(const address &o) const = 0;
            virtual bool operator>=(const address &o) const;
        };

        class address_v4 : public address
        {
        public:
            address_v4();

            /**
             * Construct by ipv4 dotted decimal string and CIDR prefix
             * @e.g: address_v4("127.0.0.1") or address_v4("127.0.0.1/8")
             * @e.g: address_v4("127.0.0.1", 8)
             * @e.g: address_v4("127.0.0.1", "255.0.0.0")
             * Accept some special address format
             * @e.g: "127" -> "127.0.0.0"
             * @e.g: "127.1" -> "127.0.0.1"
             * @e.g: "192.168.1" -> "192.168.0.1"
             * @caution default CIDR prefix will be 32 if you don't provide
             */
            address_v4(const std::string &addr);
            address_v4(const std::string &addr, std::uint8_t cidr);
            address_v4(const std::string &addr, const std::string &mask);

            /**
             * Construct by ipv4 integer and CIDR prefix
             * @e.g: "127.0.0.1" is 0x7F000001, also is 2130706433
             * @e.g: address_v4(0x7F000001)
             * @e.g: address_v4(0x7F000001, 8)
             * @e.g: address_v4(0x7F000001, "255.0.0.0")
             * @caution default CIDR prefix will be 32 if you don't provide
             */
            address_v4(std::uint32_t addr);
            address_v4(std::uint32_t addr, std::uint8_t cidr);
            address_v4(std::uint32_t addr, const std::string &mask);

            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const override;

        public:
            /**
             * Standard canonical representation
             * @e.g: 127.0.0.1
             */
            virtual std::string str() const override;

            /**
             * CIDR prefix notation representation
             * @e.g: 127.0.0.1/8
             */
            std::string notation() const;

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

        public:
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
             * @caution consider CIDR prefix when address is equal
             */
            virtual bool operator==(const address &o) const override;
            virtual bool operator<(const address &o) const override;
            virtual bool operator<=(const address &o) const override;

        public:
            /**
             * Convert between integer and string
             * @caution default CIDR prefix will be 32 if you don't provide
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
        public:
            address_v6();

            /**
             * Construct by ipv6 address, accept the following format:
             * :-) ::
             * :-) ::1
             * :-) 0:0:0:0:0:0:0:1
             * :-) 2404:6800:4004:817::200e
             * :-) 2404:6800:4004:817:0:0:0:200e
             * :-) 2404:6800:4004:817:0000:0000:0000:200e
             * :-) ::192.168.0.1
             * Also accept CIDR prefix like:
             * :-) 2404:6800:4004:817::200e/64
             * @caution default CIDR prefix will be 128 if you don't provide
             */
            address_v6(const std::string &addr);
            address_v6(const std::string &addr, std::uint8_t cidr);

            /**
             * Construct by bytes array
             * @caution default CIDR prefix will be 128 if you don't provide
             */
            address_v6(const std::array<std::uint8_t, 16> &addr);
            address_v6(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr);

            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const override;

        public:
            /**
             * Standard canonical representation, same to method compressed()
             */
            virtual std::string str() const override;

            /**
             * CIDR prefix notation representation
             * @e.g: 2404:6800:4004:817::200e/64
             */
            std::string notation() const;

            /**
             * Expanded representation, no compressed
             * @e.g: 2404:6800:4004:817:0000:0000:0000:200e
             */
            std::string expanded() const;

            /**
             * Leading zero suppressed representation
             * @e.g: 2404:6800:4004:817:0:0:0:200e
             */
            std::string suppressed() const;

            /**
             * Zero compressed representation
             * @e.g: 2404:6800:4004:817::200e
             * @e.g: ::
             */
            std::string compressed() const;

            /**
             * Mixed representation, used for IPv4-mapped address
             * @e.g: ::ffff:192.0.2.128
             */
            std::string mixed() const;

            /**
             * Retrieve IPv4-mapped or IPv4-compatible address
             */
            address_v4 v4() const;

            /**
             * Get raw value
             */
            const std::array<std::uint8_t, 16>& addr() const;
            std::uint8_t cidr() const;

            /**
             * Netmask & Wildcard mask
             * @caution use uint128 if supported in the future
             */
            std::array<std::uint8_t, 16> netmask() const;
            std::array<std::uint8_t, 16> wildcard() const;

        public:
            /**
             * Network address based on current ip
             */
            address_v6 network() const;
            address_v6 minhost() const;
            address_v6 maxhost() const;

        public:
            /**
             * Special addressing type
             * @link https://tools.ietf.org/html/rfc4291
             * @link https://en.wikipedia.org/wiki/IPv6_address#Special_addresses
             * @link https://technet.microsoft.com/en-us/library/cc757359(v=ws.10).aspx
             * @link http://www.tcpipguide.com/free/t_IPv6SpecialAddressesReservedPrivateLinkLocalSiteLo.htm
             */
            bool isUnspecified() const;
            bool isLoopback() const;

            bool isGlobalUnicast() const;  // currently only 2000::/3 is being delegated by the IANA
            bool isLinkLocalUnicast() const;
            bool isSiteLocalUnicast() const;

            bool isIPv4Compatible() const;
            bool isIPv4Mapped() const;

            bool isMulticast() const;

        public:
            /**
             * Operator
             */
            virtual bool operator==(const address &o) const override;
            virtual bool operator<(const address &o) const override;
            virtual bool operator<=(const address &o) const override;

        public:
            /**
             * Convert between integer and string
             * @caution default CIDR prefix will be 128 if you don't provide
             */
            static std::string toString(const std::array<std::uint8_t, 16> &addr);
            static std::string toString(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr);

            static std::string toExpanded(const std::array<std::uint8_t, 16> &addr);
            static std::string toSuppressed(const std::array<std::uint8_t, 16> &addr);
            static std::string toCompressed(const std::array<std::uint8_t, 16> &addr);
            static std::string toMixed(const std::array<std::uint8_t, 16> &addr);

            static std::array<std::uint8_t, 16> toBytes(const std::string &addr);
            static std::array<std::uint8_t, 16> toBytes(const std::string &addr, std::uint8_t &cidr);

        protected:
            /**
             * Compress data
             */
            static std::string compress(std::array<std::uint8_t, 16>::const_iterator beg,
                                        std::array<std::uint8_t, 16>::const_iterator end);

        protected:
            std::array<std::uint8_t, 16> _addr = {};
            std::uint8_t _cidr = 0;
        };
    }
}