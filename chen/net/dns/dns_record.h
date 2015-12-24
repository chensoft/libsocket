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
            std::uint32_t address = 0;
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
            std::uint32_t serial  = 0;  // serial number
            std::int32_t refresh  = 0;  // zone refresh interval
            std::int32_t retry    = 0;  // retry interval when refresh failed
            std::int32_t expire   = 0;  // expire interval of the zone
            std::uint32_t minimum = 0;  // minimum ttl
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
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::vector<std::uint8_t> anything;
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
            std::uint32_t address = 0;
            std::uint8_t protocol = 0;
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
            std::string sa;
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
        // NSAPPTR(rfc1348)
        struct NSAPPTR : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string owner;
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
            std::uint16_t type_covered = 0;
            std::uint8_t algorithm    = 0;
            std::uint8_t labels       = 0;
            std::uint32_t original    = 0;  // original ttl
            std::uint32_t expiration  = 0;  // signature expiration
            std::uint32_t inception   = 0;  // signature inception
            std::uint16_t key_tag     = 0;
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
            std::uint8_t version   = 0;
            std::uint8_t size      = 0;
            std::uint8_t horiz_pre = 0;
            std::uint8_t vert_pre  = 0;

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
            std::string next_domain;
            std::vector<std::uint8_t> type_bitmap;
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
            std::uint16_t key_tag  = 0;
            std::uint8_t algorithm = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // A6(OBSOLETE - use AAAA)
        struct A6 : public AAAA
        {
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
        // SINK(draft-eastlake-kitchen-sink-02)
        struct SINK : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t coding    = 0;
            std::uint8_t subcoding = 0;
            std::vector<std::uint8_t> sdata;
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
            std::uint16_t key_tag   = 0;
            std::uint8_t algorithm  = 0;
            std::uint8_t digest_type = 0;
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
            std::uint8_t precedence   = 0;
            std::uint8_t gateway_type = 0;  // 0: no type, gateway should be '.', 1: ipv4, 4 bytes, 2: ipv6, 16 bytes, 3: domain name
            std::uint8_t algorithm    = 0;
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
            std::uint16_t type_covered = 0;
            std::uint8_t algorithm    = 0;
            std::uint8_t labels       = 0;
            std::uint32_t original    = 0;  // original ttl
            std::uint32_t expiration  = 0;  // signature expiration
            std::uint32_t inception   = 0;  // signature inception
            std::uint16_t key_tag     = 0;
            std::string signer;  // signer's name
            std::string signature;
        };


        // ---------------------------------------------------------------------
        // NSEC(rfc4034, section 4.1)
        struct NSEC : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string next_domain;
            std::vector<std::uint8_t> type_bitmap;
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
            std::uint8_t salt_length = 0;
            std::vector<std::uint8_t> salt;
            std::uint8_t hash_length = 0;
            std::string next_owner;
            std::vector<std::uint8_t> type_bitmap;
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
            std::uint8_t salt_length = 0;
            std::vector<std::uint8_t> salt;
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
            std::uint8_t usage         = 0;
            std::uint8_t selector      = 0;
            std::uint8_t matching_type = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // SMIMEA(draft-ietf-dane-smime, section 2)
        struct SMIMEA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t usage         = 0;
            std::uint8_t selector      = 0;
            std::uint8_t matching_type = 0;
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
            std::uint8_t hit_length   = 0;
            std::uint8_t pk_algorithm = 0;
            std::uint16_t pk_length   = 0;
            std::string hit;
            std::string publickey;
            std::string rendezvous_servers;
        };


        // ---------------------------------------------------------------------
        // NINFO(draft-lewis-dns-undocumented-types-01)
        struct NINFO : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string zs_data;
        };


        // ---------------------------------------------------------------------
        // RKEY(draft-lewis-dns-undocumented-types-01)
        struct RKEY : public RR
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
        // TALINK(draft-lewis-dns-undocumented-types-01)
        struct TALINK : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string previous_name;
            std::string next_name;
        };


        // ---------------------------------------------------------------------
        // CDS(rfc7344, section 3.1)
        struct CDS : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // CDNSKEY(rfc7344, section 3.2)
        struct CDNSKEY : public RR
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
        // OPENPGPKEY(draft-ietf-dane-openpgpkey, section 2.1)
        struct OPENPGPKEY : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // CSYNC(rfc7477, section 2.1.1)
        struct CSYNC : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint32_t serial = 0;
            std::uint16_t flags  = 0;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // SPF(rfc4408, section 3)
        struct SPF : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string txt;
        };


        // ---------------------------------------------------------------------
        // UINFO(IANA reserved, no RFC documented)
        struct UINFO : public Unknown
        {
        };


        // ---------------------------------------------------------------------
        // UID(IANA reserved, no RFC documented)
        struct UID : public Unknown
        {
        };


        // ---------------------------------------------------------------------
        // GID(IANA reserved, no RFC documented)
        struct GID : public Unknown
        {
        };


        // ---------------------------------------------------------------------
        // UNSPEC(IANA reserved, no RFC documented)
        struct UNSPEC : public Unknown
        {
        };


        // ---------------------------------------------------------------------
        // NID(rfc6742, section 2.1.1)
        struct NID : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t preference = 0;
            std::uint64_t node_id    = 0;
        };


        // ---------------------------------------------------------------------
        // L32(rfc6742, section 2.2.1)
        struct L32 : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t preference = 0;
            std::uint32_t locator32  = 0;
        };


        // ---------------------------------------------------------------------
        // L64(rfc6742, section 2.3.1)
        struct L64 : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t preference = 0;
            std::uint32_t locator64  = 0;
        };


        // ---------------------------------------------------------------------
        // LP(rfc6742, section 2.4.1)
        struct LP : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t preference = 0;
            std::string fqdn;
        };


        // ---------------------------------------------------------------------
        // EUI48(rfc7043, section 3.1)
        struct EUI48 : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::array<std::uint8_t, 6> address;
        };


        // ---------------------------------------------------------------------
        // EUI64(rfc7043, section 4.1)
        struct EUI64 : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint64_t address = 0;
        };


        // ---------------------------------------------------------------------
        // TKEY(rfc2930, section 2)
        struct TKEY : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string algorithm;
            std::uint32_t inception  = 0;
            std::uint32_t expiration = 0;
            std::uint16_t mode       = 0;
            std::uint16_t error      = 0;
            std::uint16_t key_size   = 0;
            std::vector<std::uint8_t> key;
            std::uint16_t other_len  = 0;
            std::vector<std::uint8_t> other_data;
        };


        // ---------------------------------------------------------------------
        // TSIG(rfc2845, section 2.3)
        struct TSIG : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::string algorithm;
            std::array<std::uint8_t, 6> time_signed;
            std::uint16_t fudge    = 0;
            std::uint16_t mac_size = 0;
            std::vector<std::uint8_t> mac;
            std::uint16_t original_id = 0;
            std::uint16_t error       = 0;
            std::uint16_t other_len   = 0;
            std::vector<std::uint8_t> other_data;
        };


        // ---------------------------------------------------------------------
        // URI(rfc7553, section 4.5)
        struct URI : public RR
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
            std::string target;
        };


        // ---------------------------------------------------------------------
        // CAA(rfc6844, section 5.1.1)
        struct CAA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint8_t flags = 0;
            std::string tag;
            std::string value;
        };


        // ---------------------------------------------------------------------
        // TA(Deploying DNSSEC Without a Signed Root)
        struct TA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // DLV(rfc4431, section 2)
        struct DLV : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const override;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };
    }
}