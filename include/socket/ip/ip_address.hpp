/**
 * Created by Jian Chen
 * @since  2016.07.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <array>

namespace chen
{
    namespace ip
    {
        class address_v4;
        class address_v6;

        // ---------------------------------------------------------------------
        // Abstract address
        class address
        {
        public:
            address() = default;
            address(std::uint8_t cidr);

            virtual ~address() = default;

            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const = 0;

        public:
            /**
             * Assignment
             */
            virtual void assign(const std::string &addr) = 0;
            virtual void assign(const std::string &addr, std::uint8_t cidr) = 0;
            virtual void assign(const std::string &addr, const std::string &mask) = 0;

            virtual address& operator=(const std::string &addr) = 0;

        public:
            /**
             * Standard representation
             * with CIDR notation or not
             */
            virtual std::string str(bool cidr = false) const = 0;

            /**
             * Binary representation
             */
            virtual std::vector<std::uint8_t> bytes() const = 0;

            /**
             * Get raw value
             */
            std::uint8_t cidr() const;
            void cidr(std::uint8_t value);

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

        public:
            /**
             * Convert base address to its real type
             * @result nullptr if type is mismatch
             */
            const address_v4* v4() const;
            const address_v6* v6() const;

            address_v4* v4();
            address_v6* v6();

        public:
            /**
             * Generate IPv4 or ipv6 address automatically
             */
            static std::shared_ptr<chen::ip::address> create(const std::string &addr);
            static std::shared_ptr<chen::ip::address> create(const std::string &addr, std::uint8_t cidr);

            /**
             * Detect IPv4 or ipv6 according to string
             */
            static bool isIPv4(const std::string &addr);
            static bool isIPv6(const std::string &addr);

        protected:
            /**
             * Allow copy in derived class
             */
            address(const address&) = default;
            address& operator=(const address&) = default;

        protected:
            std::uint8_t _cidr;  // CIDR notation prefix length
        };


        // ---------------------------------------------------------------------
        // IPv4 address
        class address_v4 : public address
        {
        public:
            address_v4();

            /**
             * Construct by IPv4 dotted decimal string and CIDR prefix
             * :-) address_v4("127.0.0.1")
             * :-) address_v4("127.0.0.1/8")
             * :-) address_v4("127.0.0.1", 8)
             * :-) address_v4("127.0.0.1", "255.0.0.0")
             * Accept some special address format
             * :-) address_v4("127")       -> "127.0.0.0"
             * :-) address_v4("127.1")     -> "127.0.0.1"
             * :-) address_v4("192.168.1") -> "192.168.0.1"
             * @caution default CIDR prefix will be 32 if you don't provide
             */
            address_v4(const std::string &addr);
            address_v4(const std::string &addr, std::uint8_t cidr);
            address_v4(const std::string &addr, const std::string &mask);

            /**
             * Construct by IPv4 integer and CIDR prefix
             * e.g: "127.0.0.1" is 0x7F000001, also is 2130706433
             * :-) address_v4(2130706433)
             * :-) address_v4(0x7F000001)
             * :-) address_v4(0x7F000001, 8)
             * :-) address_v4(0x7F000001, "255.0.0.0")
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
             * Assignment
             */
            virtual void assign(const std::string &addr) override;
            virtual void assign(const std::string &addr, std::uint8_t cidr) override;
            virtual void assign(const std::string &addr, const std::string &mask) override;

            virtual void assign(std::uint32_t addr);
            virtual void assign(std::uint32_t addr, std::uint8_t cidr);
            virtual void assign(std::uint32_t addr, const std::string &mask);

            virtual address& operator=(const std::string &addr) override;
            virtual address& operator=(std::uint32_t addr);

        public:
            /**
             * Standard representation
             * e.g: 127.0.0.1
             * e.g: 127.0.0.1/8
             */
            virtual std::string str(bool cidr = false) const override;

            /**
             * Binary representation
             */
            virtual std::vector<std::uint8_t> bytes() const override;

            /**
             * Get raw value
             */
            std::uint32_t addr() const;
            void addr(std::uint32_t value);

        public:
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
             * Operator, compare based on IPv4 address
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
            static std::uint32_t toInteger(const std::string &addr, std::uint8_t *cidr);

            static std::uint8_t toCIDR(const std::string &mask);
            static std::uint8_t toCIDR(std::uint32_t mask);

        public:
            /**
             * Common addresses
             */
            static address_v4 any();
            static address_v4 broad();
            static address_v4 loopback();

        protected:
            std::uint32_t _addr = 0;  // 32 bits IPv4 address, little-endian
        };


        // ---------------------------------------------------------------------
        // IPv6 address
        class address_v6 : public address
        {
        public:
            address_v6();

            /**
             * Construct by ipv6 address, accept the following format:
             * :-) address_v6("::")
             * :-) address_v6("::1")
             * :-) address_v6("0:0:0:0:0:0:0:1")
             * :-) address_v6("2404:6800:4004:817::200e")
             * :-) address_v6("2404:6800:4004:817:0:0:0:200e")
             * :-) address_v6("2404:6800:4004:817:0000:0000:0000:200e")
             * :-) address_v6("::192.168.0.1")
             * Also accept CIDR prefix like:
             * :-) address_v6("2404:6800:4004:817::200e/64")
             * :-) address_v6("2404:6800:4004:817::200e", 64)
             * :-) address_v6("2404:6800:4004:817::200e", "ffff:ffff:ffff:ffff::")
             * And accept scope id like:
             * :-) address_v6("fe80::1%1")
             * :-) address_v6("fe80::1%lo0")
             * :-) address_v6("fe80::1%1/64")
             * :-) address_v6("fe80::1%lo0/64")
             * :-) address_v6("fe80::1%lo0", 64)
             * :-) address_v6("fe80::1%lo0", "ffff:ffff:ffff:ffff::")
             * @caution default CIDR prefix will be 128 if you don't provide
             */
            address_v6(const std::string &addr);
            address_v6(const std::string &addr, std::uint8_t cidr);
            address_v6(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
            address_v6(const std::string &addr, const std::string &mask);
            address_v6(const std::string &addr, const std::string &mask, std::uint32_t scope);

            /**
             * Construct by bytes array
             * @caution default CIDR prefix will be 128 if you don't provide
             */
            address_v6(const std::array<std::uint8_t, 16> &addr);
            address_v6(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr);
            address_v6(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr, std::uint32_t scope);
            address_v6(const std::array<std::uint8_t, 16> &addr, const std::string &mask);
            address_v6(const std::array<std::uint8_t, 16> &addr, const std::string &mask, std::uint32_t scope);

            address_v6(const std::uint8_t addr[16]);
            address_v6(const std::uint8_t addr[16], std::uint8_t cidr);
            address_v6(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope);
            address_v6(const std::uint8_t addr[16], const std::string &mask);
            address_v6(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope);

            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const override;

        public:
            /**
             * Assignment
             */
            virtual void assign(const std::string &addr) override;
            virtual void assign(const std::string &addr, std::uint8_t cidr) override;
            virtual void assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
            virtual void assign(const std::string &addr, const std::string &mask) override;
            virtual void assign(const std::string &addr, const std::string &mask, std::uint32_t scope);

            virtual void assign(const std::array<std::uint8_t, 16> &addr);
            virtual void assign(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr);
            virtual void assign(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr, std::uint32_t scope);
            virtual void assign(const std::array<std::uint8_t, 16> &addr, const std::string &mask);
            virtual void assign(const std::array<std::uint8_t, 16> &addr, const std::string &mask, std::uint32_t scope);

            virtual void assign(const std::uint8_t addr[16]);
            virtual void assign(const std::uint8_t addr[16], std::uint8_t cidr);
            virtual void assign(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope);
            virtual void assign(const std::uint8_t addr[16], const std::string &mask);
            virtual void assign(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope);

            virtual address& operator=(const std::string &addr) override;
            virtual address& operator=(const std::array<std::uint8_t, 16> &addr);
            virtual address& operator=(const std::uint8_t addr[16]);

        public:
            /**
             * Standard representation, same to the method compressed()
             * e.g: 2404:6800:4004:817::200e
             * e.g: 2404:6800:4004:817::200e/64
             */
            virtual std::string str(bool cidr = false) const override;

            /**
             * Standard representation, with scope id
             * e.g: fe80::1%1      if interface name is not found
             * e.g: fe80::1%lo0    if scope id to interface name is found
             * e.g: fe80::1%lo0/64 if cidr is enabled
             */
            virtual std::string str(bool cidr, bool scope) const;

            /**
             * Binary representation
             */
            virtual std::vector<std::uint8_t> bytes() const override;

            /**
             * Expanded representation, no compressed
             * e.g: 2404:6800:4004:817:0000:0000:0000:200e
             */
            std::string expanded() const;

            /**
             * Leading zero suppressed representation
             * e.g: 2404:6800:4004:817:0:0:0:200e
             */
            std::string suppressed() const;

            /**
             * Zero compressed representation
             * e.g: 2404:6800:4004:817::200e
             * e.g: ::
             */
            std::string compressed() const;

            /**
             * Mixed representation, used for IPv4-mapped address
             * e.g: ::ffff:192.0.2.128
             */
            std::string mixed() const;

            /**
             * Retrieve IPv4-mapped or IPv4-compatible address
             * or IPv4-embedded address according to its CIDR length
             */
            address_v4 embedded() const;

            /**
             * Get raw value
             */
            const std::array<std::uint8_t, 16>& addr() const;
            void addr(const std::array<std::uint8_t, 16>& value);

            std::uint32_t scope() const;
            void scope(std::uint32_t value);

        public:
            /**
             * Netmask & Wildcard mask
             * @caution use uint128 if supported in the future
             */
            std::array<std::uint8_t, 16> netmask() const;
            std::array<std::uint8_t, 16> wildcard() const;

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

            bool isIPv4Compatible() const;  // rfc4291
            bool isIPv4Mapped() const;      // rfc4291

            bool isMulticast() const;

            /**
             * NAT64(IPv6-only network) well-known prefix
             * "64:ff9b::/96" is the well-known prefix for IPv4-Embedded address
             * e.g: v4 "115.182.64.94" -> v6 "64:ff9b::73b6:405e/96", aka "64:ff9b::115.182.64.94/96"
             * @link https://tools.ietf.org/html/rfc6052
             * @link https://developer.apple.com/library/mac/documentation/NetworkingInternetWeb/Conceptual/NetworkingOverview/UnderstandingandPreparingfortheIPv6Transition/UnderstandingandPreparingfortheIPv6Transition.html#//apple_ref/doc/uid/TP40010220-CH213-SW1
             */
            bool isIPv4EmbeddedWellKnown() const;

        public:
            /**
             * Operator
             * @caution consider CIDR prefix when address is equal
             */
            virtual bool operator==(const address &o) const override;
            virtual bool operator<(const address &o) const override;
            virtual bool operator<=(const address &o) const override;

        public:
            /**
             * Convert between bytes and string
             * @caution default CIDR prefix will be 128 if you don't provide
             */
            static std::string toString(const std::uint8_t addr[16]);
            static std::string toString(const std::uint8_t addr[16], std::uint8_t cidr);

            static std::string toScope(const std::uint8_t addr[16], std::uint32_t scope);
            static std::string toScope(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope);

            static std::string toExpanded(const std::uint8_t addr[16]);
            static std::string toSuppressed(const std::uint8_t addr[16]);
            static std::string toCompressed(const std::uint8_t addr[16]);
            static std::string toMixed(const std::uint8_t addr[16]);

            static std::array<std::uint8_t, 16> toBytes(const std::string &addr);
            static std::array<std::uint8_t, 16> toBytes(const std::string &addr, std::uint8_t *cidr);
            static std::array<std::uint8_t, 16> toBytes(const std::string &addr, std::uint8_t *cidr, std::uint32_t *scope);

            static std::uint8_t toCIDR(const std::string &mask);
            static std::uint8_t toCIDR(const std::uint8_t mask[16]);

        public:
            /**
             * Common addresses
             */
            static address_v6 any();
            static address_v6 loopback();

            /**
             * Compress data
             */
            static std::string compress(std::array<std::uint8_t, 16>::const_iterator beg,
                                        std::array<std::uint8_t, 16>::const_iterator end);

        protected:
            std::array<std::uint8_t, 16> _addr = {};
            std::uint32_t _scope = 0;  // link-local address's(e.g: fe80::1%lo0) scope zone id, rfc4007, section 11
        };
    }
}