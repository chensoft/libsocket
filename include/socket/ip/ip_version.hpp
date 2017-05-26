/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace chen
{
    // -------------------------------------------------------------------------
    // Base
    class ip_version
    {
    public:
        /**
         * Get raw value
         */
        std::uint8_t cidr() const;
        void cidr(std::uint8_t value);

    protected:
        std::uint8_t _cidr;  // CIDR notation prefix length
    };


    // -------------------------------------------------------------------------
    // IPv4
    class ip_version4 : public ip_version
    {
        friend class ip_address;
        ip_version4() = default;  // trivial constructor, used by address only

    public:
        explicit ip_version4(const std::string &addr);
        ip_version4(const std::string &addr, std::uint8_t cidr);
        ip_version4(const std::string &addr, const std::string &mask);

        explicit ip_version4(std::uint32_t addr);
        ip_version4(std::uint32_t addr, std::uint8_t cidr);
        ip_version4(std::uint32_t addr, const std::string &mask);

    public:
        /**
         * Assign empty address
         */
        void assign();

        /**
         * Assign by IPv4 dotted decimal string and CIDR prefix
         * :-) ip_version4("127.0.0.1")
         * :-) ip_version4("127.0.0.1/8")
         * :-) ip_version4("127.0.0.1", 8)
         * :-) ip_version4("127.0.0.1", "255.0.0.0")
         * Accept some special address format
         * :-) ip_version4("127")       -> "127.0.0.0"
         * :-) ip_version4("127.1")     -> "127.0.0.1"
         * :-) ip_version4("192.168.1") -> "192.168.0.1"
         * @note default CIDR prefix will be 32 if you don't provide
         */
        void assign(const std::string &addr);
        void assign(const std::string &addr, std::uint8_t cidr);
        void assign(const std::string &addr, const std::string &mask);

        /**
         * Assign by IPv4 integer and CIDR prefix
         * e.g: "127.0.0.1" is 0x7f000001, also is 2130706433
         * :-) ip_version4(2130706433)
         * :-) ip_version4(0x7f000001)
         * :-) ip_version4(0x7f000001, 8)
         * :-) ip_version4(0x7f000001, "255.0.0.0")
         * @note default CIDR prefix will be 32 if you don't provide
         */
        void assign(std::uint32_t addr);
        void assign(std::uint32_t addr, std::uint8_t cidr);
        void assign(std::uint32_t addr, const std::string &mask);

        /**
         * Assignment operator
         */
        ip_version4& operator=(const std::string &addr);
        ip_version4& operator=(std::uint32_t addr);

    public:
        /**
         * Standard representation
         * e.g: 127.0.0.1
         * e.g: 127.0.0.1/8
         */
        std::string str(bool cidr = false) const;

        /**
         * Binary representation
         */
        std::vector<std::uint8_t> bytes() const;

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
        ip_version4 network() const;
        ip_version4 minhost() const;
        ip_version4 maxhost() const;
        ip_version4 broadcast() const;

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
         * @note since CIDR is widely used today, classful is not very useful now
         */
        bool isClassA() const;
        bool isClassB() const;
        bool isClassC() const;
        bool isClassD() const;
        bool isClassE() const;

    public:
        /**
         * Operator, compare based on IPv4 address
         * @note consider CIDR prefix when address is equal
         */
        bool operator==(const ip_version4 &o) const;
        bool operator!=(const ip_version4 &o) const;

        bool operator<(const ip_version4 &o) const;
        bool operator>(const ip_version4 &o) const;

        bool operator<=(const ip_version4 &o) const;
        bool operator>=(const ip_version4 &o) const;

    public:
        /**
         * Convert between integer and string
         * @note default CIDR prefix will be 32 if you don't provide
         */
        static std::string toString(std::uint32_t addr);
        static std::string toString(std::uint32_t addr, std::uint8_t cidr);

        static std::uint32_t toInteger(const std::string &addr);
        static std::uint32_t toInteger(const std::string &addr, std::uint8_t *cidr);

        static std::uint8_t toCIDR(const std::string &mask);
        static std::uint8_t toCIDR(std::uint32_t mask);

    private:
        std::uint32_t _addr;  // 32 bits IPv4 address, little-endian
    };


    // -------------------------------------------------------------------------
    // IPv6
    class ip_version6 : public ip_version
    {
        friend class ip_address;
        ip_version6() = default;  // trivial constructor, used by address only

    public:
        explicit ip_version6(const std::string &addr);
        ip_version6(const std::string &addr, std::uint8_t cidr);
        ip_version6(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
        ip_version6(const std::string &addr, const std::string &mask);
        ip_version6(const std::string &addr, const std::string &mask, std::uint32_t scope);

        explicit ip_version6(const std::uint8_t addr[16]);
        ip_version6(const std::uint8_t addr[16], std::uint8_t cidr);
        ip_version6(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope);
        ip_version6(const std::uint8_t addr[16], const std::string &mask);
        ip_version6(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope);

    public:
        /**
         * Assign empty address
         */
        void assign();

        /**
         * Assign by IPv6 address, accept the following format:
         * :-) ip_version6("::")
         * :-) ip_version6("::1")
         * :-) ip_version6("0:0:0:0:0:0:0:1")
         * :-) ip_version6("2404:6800:4004:817::200e")
         * :-) ip_version6("2404:6800:4004:817:0:0:0:200e")
         * :-) ip_version6("2404:6800:4004:817:0000:0000:0000:200e")
         * :-) ip_version6("::192.168.0.1")
         * Also accept CIDR prefix like:
         * :-) ip_version6("2404:6800:4004:817::200e/64")
         * :-) ip_version6("2404:6800:4004:817::200e", 64)
         * :-) ip_version6("2404:6800:4004:817::200e", "ffff:ffff:ffff:ffff::")
         * And accept scope id like:
         * :-) ip_version6("fe80::1%1")
         * :-) ip_version6("fe80::1%lo0")
         * :-) ip_version6("fe80::1%1/64")
         * :-) ip_version6("fe80::1%lo0/64")
         * :-) ip_version6("fe80::1%lo0", 64)
         * :-) ip_version6("fe80::1%lo0", "ffff:ffff:ffff:ffff::")
         * @note default CIDR prefix will be 128 if you don't provide
         */
        void assign(const std::string &addr);
        void assign(const std::string &addr, std::uint8_t cidr);
        void assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope);
        void assign(const std::string &addr, const std::string &mask);
        void assign(const std::string &addr, const std::string &mask, std::uint32_t scope);

        /**
         * Construct by bytes array
         * @note default CIDR prefix will be 128 if you don't provide
         */
        void assign(const std::uint8_t addr[16]);
        void assign(const std::uint8_t addr[16], std::uint8_t cidr);
        void assign(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope);
        void assign(const std::uint8_t addr[16], const std::string &mask);
        void assign(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope);

        /**
         * Assignment operator
         */
        ip_version6& operator=(const std::string &addr);
        ip_version6& operator=(const std::uint8_t addr[16]);

    public:
        /**
         * Standard representation, same to the method compressed()
         * e.g: 2404:6800:4004:817::200e
         * e.g: 2404:6800:4004:817::200e/64
         */
        std::string str(bool cidr = false) const;

        /**
         * Standard representation, with scope id
         * e.g: fe80::1%1      if interface name is not found
         * e.g: fe80::1%lo0    if scope id to interface name is found
         * e.g: fe80::1%lo0/64 if cidr is enabled
         */
        std::string str(bool cidr, bool scope) const;

        /**
         * Binary representation
         */
        std::vector<std::uint8_t> bytes() const;

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
        ip_version4 embedded() const;

        /**
         * Get raw value
         */
        const std::array<std::uint8_t, 16>& addr() const;
        void addr(const std::uint8_t value[16]);

        std::uint32_t scope() const;
        void scope(std::uint32_t value);

    public:
        /**
         * Netmask & Wildcard mask
         * @note use uint128 if supported in the future
         */
        std::array<std::uint8_t, 16> netmask() const;
        std::array<std::uint8_t, 16> wildcard() const;

        /**
         * Network address based on current ip
         */
        ip_version6 network() const;
        ip_version6 minhost() const;
        ip_version6 maxhost() const;

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
         * @note consider CIDR prefix when address is equal
         */
        bool operator==(const ip_version6 &o) const;
        bool operator!=(const ip_version6 &o) const;

        bool operator<(const ip_version6 &o) const;
        bool operator>(const ip_version6 &o) const;

        bool operator<=(const ip_version6 &o) const;
        bool operator>=(const ip_version6 &o) const;

    public:
        /**
         * Convert between bytes and string
         * @note default CIDR prefix will be 128 if you don't provide
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
         * Compress data
         */
        static std::string compress(const std::uint8_t *beg, const std::uint8_t *end);

    private:
        std::array<std::uint8_t, 16> _addr;
        std::uint32_t _scope;  // link-local address's(e.g: fe80::1%lo0) scope zone id, rfc4007, section 11
    };
}