/**
 * Created by Jian Chen
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_define.h"
#include <string>
#include <vector>
#include <array>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // Resource Record(rfc1035, section 3.2.1)
        struct RR
        {
        public:
            virtual ~RR() = 0;

        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const = 0;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) = 0;

        public:
            std::string name;
            chen::dns::RRType rrtype   = chen::dns::RRType::None;
            chen::dns::RRClass rrclass = chen::dns::RRClass::IN;
            std::int32_t ttl = 0;

            // todo put rdlength here, the rr can use this length to get data from buffer
        };


        // ---------------------------------------------------------------------
        // Unknown
        struct Unknown : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t rdlength = 0;
            std::vector<std::uint8_t> rdata;
        };


        // ---------------------------------------------------------------------
        // A(rfc1035, section 3.4.1)
        struct A : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::int32_t address = 0;
        };


        // ---------------------------------------------------------------------
        // NS(rfc1035, section 3.3.11)
        struct NS : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string nsdname;
        };


        // ---------------------------------------------------------------------
        // MD(rfc1035, section 3.3.4)
        struct MD : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // MF(rfc1035, section 3.3.5)
        struct MF : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // CNAME(rfc1035, section 3.3.1)
        struct CNAME : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string cname;
        };


        // ---------------------------------------------------------------------
        // SOA(rfc1035, section 3.3.13)
        struct SOA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string mname;  // primary nameserver
            std::string rname;  // hostmaster email address
            std::uint32_t serial = 0;  // serial number
            std::int32_t refresh = 0;  // zone refresh interval
            std::int32_t retry   = 0;  // retry interval when refresh failed
            std::int32_t expire  = 0;  // expire interval of the zone
        };


        // ---------------------------------------------------------------------
        // MB(rfc1035, section 3.3.3)
        struct MB : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // MG(rfc1035, section 3.3.6)
        struct MG : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string mgmname;
        };


        // ---------------------------------------------------------------------
        // MR(rfc1035, section 3.3.8)
        struct MR : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string newname;
        };


        // ---------------------------------------------------------------------
        // RNULL(rfc1035, section 3.3.10, can store anything)
        struct RNULL : public Unknown
        {
        };


        // ---------------------------------------------------------------------
        // WKS(rfc1035, section 3.4.2)
        struct WKS : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::int32_t address = 0;
            std::int8_t protocol = 0;
            std::vector<std::uint8_t> bitmap;
        };


        // ---------------------------------------------------------------------
        // PTR(rfc1035, section 3.3.12)
        struct PTR : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string ptrdname;
        };


        // ---------------------------------------------------------------------
        // HINFO(rfc1035, section 3.3.2)
        struct HINFO : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string cpu;
            std::string os;
        };


        // ---------------------------------------------------------------------
        // MINFO(rfc1035, section 3.3.7)
        struct MINFO : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string rmailbx;
            std::string emailbx;
        };


        // ---------------------------------------------------------------------
        // MX(rfc1035, section 3.3.9)
        struct MX : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::int16_t preference = 0;
            std::string exchange;
        };


        // ---------------------------------------------------------------------
        // TXT(rfc1035, section 3.3.14)
        struct TXT : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string txt_data;
        };


        // ---------------------------------------------------------------------
        // RP(rfc1183, section 2.2)
        struct RP : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string mbox_dname;
            std::string txt_dname;
        };


        // ---------------------------------------------------------------------
        // AFSDB(rfc1183, section 1)
        struct AFSDB : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::int16_t subtype = 0;
            std::string hostname;
        };


        // ---------------------------------------------------------------------
        // X25(rfc1183, section 3.1)
        struct X25 : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string psdn_address;
        };


        // ---------------------------------------------------------------------
        // ISDN(rfc1183, section 3.2)
        struct ISDN : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string isdn_address;
        };


        // ---------------------------------------------------------------------
        // RT(rfc1183, section 3.3)
        struct RT : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::int16_t preference = 0;
            std::string intermediate_host;
        };


        // ---------------------------------------------------------------------
        // NSAP(rfc1706, section 5)
        struct NSAP : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string nsap;
        };


        // ---------------------------------------------------------------------
        // NSAPPTR(rfc1706, section 6)
        struct NSAPPTR : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string ptrdname;
        };


        // ---------------------------------------------------------------------
        // SIG(rfc2535, section 4.1)
        struct SIG : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t typecovered = 0;
            std::uint8_t algorithm    = 0;
            std::uint8_t labels       = 0;
            std::uint32_t original    = 0;  // original ttl
            std::uint32_t expiration  = 0;  // signature expiration
            std::uint32_t inception   = 0;  // signature inception
            std::uint16_t keytag      = 0;
            std::string signer;  // signer's name
            std::string signature;
        };


        // ---------------------------------------------------------------------
        // KEY(rfc2535, section 3.1)
        struct KEY : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // PX(rfc2163, section 4)
        struct PX : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::int16_t preference = 0;
            std::string map822;
            std::string mapx400;
        };


        // ---------------------------------------------------------------------
        // GPOS(rfc1712, section 3)
        struct GPOS : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string longitude;
            std::string latitude;
            std::string altitude;
        };


        // ---------------------------------------------------------------------
        // AAAA(rfc3596, section 2.2)
        struct AAAA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::array<std::uint8_t, 16> address;
        };


        // ---------------------------------------------------------------------
        // LOC(rfc1876, section 2)
        struct LOC : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t version  = 0;
            std::uint8_t size     = 0;
            std::uint8_t horizpre = 0;
            std::uint8_t vertpre  = 0;

            std::uint32_t longitude = 0;
            std::uint32_t latitude  = 0;
            std::uint32_t altitude  = 0;
        };


        // ---------------------------------------------------------------------
        // NXT(rfc2535, section 5.2)
        struct NXT : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string next;
            std::vector<std::uint8_t> bitmap;
        };


        // ---------------------------------------------------------------------
        // EID(draft-lewis-dns-undocumented-types-01)
        struct EID : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string endpoint;
        };


        // ---------------------------------------------------------------------
        // NIMLOC(draft-lewis-dns-undocumented-types-01)
        struct NIMLOC : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string locator;
        };


        // ---------------------------------------------------------------------
        // SRV(rfc2782)
        struct SRV : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t priority = 0;
            std::uint16_t weight   = 0;
            std::uint16_t port     = 0;
            std::string target;
        };


        // ---------------------------------------------------------------------
        // ATMA(ATM Name System Specification Version 1.0)
        struct ATMA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t format = 0;
            std::string address;
        };


        // ---------------------------------------------------------------------
        // NAPTR(rfc3403, section 4.1)
        struct NAPTR : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t order = 0;
            std::uint16_t preference = 0;
            std::string flags;
            std::string services;
            std::string regexp;
            std::string replacement;
        };


        // ---------------------------------------------------------------------
        // KX(rfc2230, section 3.1)
        struct KX : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t preference = 0;
            std::string exchanger;
        };


        // ---------------------------------------------------------------------
        // CERT(rfc4398, section 2)
        struct CERT : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t type     = 0;
            std::uint16_t keytag   = 0;
            std::uint8_t algorithm = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // DNAME(rfc2672, section 3)
        struct DNAME : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string target;
        };


        // ---------------------------------------------------------------------
        // OPT(rfc6891, section 6.1.2)
        struct OPT : public Unknown
        {
        };


        // ---------------------------------------------------------------------
        // DS(rfc4034, section 5.1)
        struct DS : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t keytag    = 0;
            std::uint8_t algorithm  = 0;
            std::uint8_t digesttype = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // SSHFP(rfc4255, section 3.1)
        struct SSHFP : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t algorithm = 0;
            std::uint8_t fptype    = 0;
            std::string fingerprint;
        };


        // ---------------------------------------------------------------------
        // IPSECKEY(rfc4025, section 2.1)
        struct IPSECKEY : public RR
        {
        public:
            enum class GatewayType {None, IPv4, IPv6, Domain};

        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t precedence  = 0;
            std::uint8_t gatewaytype = 0;  // 0: no type, gateway should be '.', 1: ipv4, 4 bytes, 2: ipv6, 16 bytes, 3: domain name
            std::uint8_t algorithm   = 0;
            std::vector<std::uint8_t> gateway;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // RRSIG(rfc4034, section 3.1)
        struct RRSIG : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t typecovered = 0;
            std::uint8_t algorithm    = 0;
            std::uint8_t labels       = 0;
            std::uint32_t original    = 0;  // original ttl
            std::uint32_t expiration  = 0;  // signature expiration
            std::uint32_t inception   = 0;  // signature inception
            std::uint16_t keytag      = 0;
            std::string signer;  // signer's name
            std::string signature;
        };


        // ---------------------------------------------------------------------
        // NSEC(rfc4034, section 3.1)
        struct NSEC : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string nextdomain;
            std::vector<std::uint8_t> typebitmap;
        };


        // ---------------------------------------------------------------------
        // DNSKEY(rfc4034, section 2.1)
        struct DNSKEY : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // DHCID(rfc4701, section 3.1)
        struct DHCID : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // NSEC3(rfc5155, section 3.2)
        struct NSEC3 : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t hash        = 0;
            std::uint8_t flags       = 0;
            std::uint16_t iterations = 0;
            std::uint8_t saltLength  = 0;
            std::string salt;
            std::uint8_t hashlength  = 0;
            std::string nextowner;
            std::vector<std::uint8_t> typebitmap;
        };


        // ---------------------------------------------------------------------
        // NSEC3PARAM(rfc5155, section 4.2)
        struct NSEC3PARAM : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t hash        = 0;
            std::uint8_t flags       = 0;
            std::uint16_t iterations = 0;
            std::uint8_t saltLength  = 0;
            std::string salt;
        };


        // ---------------------------------------------------------------------
        // TLSA(rfc6698, section 2.1)
        struct TLSA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t usage        = 0;
            std::uint8_t selector     = 0;
            std::uint8_t matchingtype = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // HIP(rfc5205, section 5)
        struct HIP : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t hitlength   = 0;
            std::uint8_t pkalgorithm = 0;
            std::uint16_t pklength   = 0;
            std::string hit;
            std::string publickey;
            std::string rendezvousservers;
        };
    }
}