/**
 * Created by Jian Chen
 * I'm very careful to ensure that records's fields are accurate
 * The fields are signed or unsigned is according to its related rfc
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_define.hpp"
#include <string>
#include <vector>
#include <memory>
#include <array>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // Resource Record(rfc1035, section 3.2.1)
        class RR
        {
        public:
            RR(chen::dns::RRType type);
            virtual ~RR() = 0;

        public:
            /**
             * Encode
             */
            virtual std::vector<std::uint8_t> encode() const;
            virtual void encode(std::vector<std::uint8_t> &out) const;

            /**
             * Decode, detect rr's type automatically
             */
            static std::shared_ptr<chen::dns::RR> decode(const std::vector<std::uint8_t> &data);
            static std::shared_ptr<chen::dns::RR> decode(std::vector<std::uint8_t>::const_iterator &cur,
                                                         std::vector<std::uint8_t>::const_iterator &end);

        public:
            /**
             * Clone current record
             */
            virtual std::shared_ptr<chen::dns::RR> clone() const = 0;

        protected:
            /**
             * Helper for encode & decode
             */
            virtual void pack(std::vector<std::uint8_t> &out) const = 0;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) = 0;

        protected:
            /**
             * Set rdlength when finish encode
             */
            virtual void adjust(std::vector<std::uint8_t> &out, std::size_t val) const;

            /**
             * Check remain data when decode
             */
            virtual std::size_t remain(std::vector<std::uint8_t>::const_iterator &beg,
                                       std::vector<std::uint8_t>::const_iterator &cur) const;

        public:
            std::string name;
            chen::dns::RRType rrtype   = chen::dns::RRType::None;
            chen::dns::RRClass rrclass = chen::dns::RRClass::IN;
            std::int32_t ttl = 0;  // rfc1035 says it's a 32 bit signed integer

        protected:
            // internal use only, will be set when call unpack method
            std::uint16_t rdlength = 0;
        };


        // ---------------------------------------------------------------------
        // Custom RR
        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        // Raw(raw resource record, with rdata)
        class Raw : public RR
        {
        public:
            Raw();
            Raw(chen::dns::RRType type);

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::vector<std::uint8_t> rdata;
        };


        // ---------------------------------------------------------------------
        // Unknown(unknown record)
        class Unknown : public Raw
        {
        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // Standard RR
        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        // A(rfc1035, section 3.4.1)
        class A : public RR
        {
        public:
            A();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint32_t address = 0;
        };


        // ---------------------------------------------------------------------
        // NS(rfc1035, section 3.3.11)
        class NS : public RR
        {
        public:
            NS();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string nsdname;
        };


        // ---------------------------------------------------------------------
        // MD(rfc1035, section 3.3.4)
        class MD : public RR
        {
        public:
            MD();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // MF(rfc1035, section 3.3.5)
        class MF : public RR
        {
        public:
            MF();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // CNAME(rfc1035, section 3.3.1)
        class CNAME : public RR
        {
        public:
            CNAME();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string cname;
        };


        // ---------------------------------------------------------------------
        // SOA(rfc1035, section 3.3.13)
        class SOA : public RR
        {
        public:
            SOA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class MB : public RR
        {
        public:
            MB();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // MG(rfc1035, section 3.3.6)
        class MG : public RR
        {
        public:
            MG();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string mgmname;
        };


        // ---------------------------------------------------------------------
        // MR(rfc1035, section 3.3.8)
        class MR : public RR
        {
        public:
            MR();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string newname;
        };


        // ---------------------------------------------------------------------
        // NULL(rfc1035, section 3.3.10, can store anything)
        class NUL : public Raw
        {
        public:
            NUL();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::vector<std::uint8_t> anything;
        };


        // ---------------------------------------------------------------------
        // WKS(rfc1035, section 3.4.2)
        class WKS : public RR
        {
        public:
            WKS();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint32_t address = 0;
            std::uint8_t protocol = 0;
            std::vector<std::uint8_t> bitmap;
        };


        // ---------------------------------------------------------------------
        // PTR(rfc1035, section 3.3.12)
        class PTR : public RR
        {
        public:
            PTR();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string ptrdname;
        };


        // ---------------------------------------------------------------------
        // HINFO(rfc1035, section 3.3.2)
        class HINFO : public RR
        {
        public:
            HINFO();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string cpu;
            std::string os;
        };


        // ---------------------------------------------------------------------
        // MINFO(rfc1035, section 3.3.7)
        class MINFO : public RR
        {
        public:
            MINFO();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string rmailbx;
            std::string emailbx;
        };


        // ---------------------------------------------------------------------
        // MX(rfc1035, section 3.3.9)
        class MX : public RR
        {
        public:
            MX();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::int16_t preference = 0;
            std::string exchange;
        };


        // ---------------------------------------------------------------------
        // TXT(rfc1035, section 3.3.14)
        class TXT : public RR
        {
        public:
            TXT();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string txt_data;
        };


        // ---------------------------------------------------------------------
        // RP(rfc1183, section 2.2)
        class RP : public RR
        {
        public:
            RP();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string mbox_dname;
            std::string txt_dname;
        };


        // ---------------------------------------------------------------------
        // AFSDB(rfc1183, section 1)
        class AFSDB : public RR
        {
        public:
            AFSDB();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::int16_t subtype = 0;
            std::string hostname;
        };


        // ---------------------------------------------------------------------
        // X25(rfc1183, section 3.1)
        class X25 : public RR
        {
        public:
            X25();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string psdn_address;
        };


        // ---------------------------------------------------------------------
        // ISDN(rfc1183, section 3.2)
        class ISDN : public RR
        {
        public:
            ISDN();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string isdn_address;
            std::string sa;
        };


        // ---------------------------------------------------------------------
        // RT(rfc1183, section 3.3)
        class RT : public RR
        {
        public:
            RT();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::int16_t preference = 0;
            std::string intermediate_host;
        };


        // ---------------------------------------------------------------------
        // NSAP(rfc1706, section 5)
        class NSAP : public RR
        {
        public:
            NSAP();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string nsap;
        };


        // ---------------------------------------------------------------------
        // NSAPPTR(rfc1348)
        class NSAPPTR : public RR
        {
        public:
            NSAPPTR();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string owner;
        };


        // ---------------------------------------------------------------------
        // SIG(rfc2535, section 4.1)
        class SIG : public RR
        {
        public:
            SIG();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t type_covered = 0;
            std::uint8_t algorithm     = 0;
            std::uint8_t labels        = 0;
            std::uint32_t original     = 0;  // original ttl
            std::uint32_t expiration   = 0;  // signature expiration
            std::uint32_t inception    = 0;  // signature inception
            std::uint16_t key_tag      = 0;
            std::string signer;  // signer's name
            std::string signature;
        };


        // ---------------------------------------------------------------------
        // KEY(rfc2535, section 3.1)
        class KEY : public RR
        {
        public:
            KEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // PX(rfc2163, section 4)
        class PX : public RR
        {
        public:
            PX();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::int16_t preference = 0;
            std::string map822;
            std::string mapx400;
        };


        // ---------------------------------------------------------------------
        // GPOS(rfc1712, section 3)
        class GPOS : public RR
        {
        public:
            GPOS();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string longitude;
            std::string latitude;
            std::string altitude;
        };


        // ---------------------------------------------------------------------
        // AAAA(rfc3596, section 2.2)
        class AAAA : public RR
        {
        public:
            AAAA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::array<std::uint8_t, 16> address;
        };


        // ---------------------------------------------------------------------
        // LOC(rfc1876, section 2)
        class LOC : public RR
        {
        public:
            LOC();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class NXT : public RR
        {
        public:
            NXT();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string next_domain;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // EID(draft-lewis-dns-undocumented-types-01)
        class EID : public RR
        {
        public:
            EID();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string endpoint;
        };


        // ---------------------------------------------------------------------
        // NIMLOC(draft-lewis-dns-undocumented-types-01)
        class NIMLOC : public RR
        {
        public:
            NIMLOC();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string locator;
        };


        // ---------------------------------------------------------------------
        // SRV(rfc2782)
        class SRV : public RR
        {
        public:
            SRV();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t priority = 0;
            std::uint16_t weight   = 0;
            std::uint16_t port     = 0;
            std::string target;
        };


        // ---------------------------------------------------------------------
        // ATMA(ATM Name System Specification Version 1.0)
        class ATMA : public RR
        {
        public:
            ATMA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t format = 0;
            std::string address;
        };


        // ---------------------------------------------------------------------
        // NAPTR(rfc3403, section 4.1)
        class NAPTR : public RR
        {
        public:
            NAPTR();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class KX : public RR
        {
        public:
            KX();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t preference = 0;
            std::string exchanger;
        };


        // ---------------------------------------------------------------------
        // CERT(rfc4398, section 2)
        class CERT : public RR
        {
        public:
            CERT();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t type     = 0;
            std::uint16_t key_tag  = 0;
            std::uint8_t algorithm = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // A6(OBSOLETE - use AAAA)
        class A6 : public AAAA
        {
        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // DNAME(rfc2672, section 3)
        class DNAME : public RR
        {
        public:
            DNAME();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string target;
        };


        // ---------------------------------------------------------------------
        // SINK(draft-eastlake-kitchen-sink-02)
        class SINK : public RR
        {
        public:
            SINK();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t coding    = 0;
            std::uint8_t subcoding = 0;
            std::vector<std::uint8_t> sdata;
        };


        // ---------------------------------------------------------------------
        // OPT(rfc6891, section 6.1.2)
        class OPT : public Raw
        {
        public:
            OPT();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // DS(rfc4034, section 5.1)
        class DS : public RR
        {
        public:
            DS();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // SSHFP(rfc4255, section 3.1)
        class SSHFP : public RR
        {
        public:
            SSHFP();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t algorithm = 0;
            std::uint8_t fptype    = 0;
            std::string fingerprint;
        };


        // ---------------------------------------------------------------------
        // IPSECKEY(rfc4025, section 2.1)
        class IPSECKEY : public RR
        {
        public:
            enum class GatewayType {None, IPv4, IPv6, Domain};

        public:
            IPSECKEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t precedence   = 0;
            std::uint8_t gateway_type = 0;  // 0: no type, gateway should be '.', 1: ipv4, 4 bytes, 2: ipv6, 16 bytes, 3: domain name
            std::uint8_t algorithm    = 0;
            std::vector<std::uint8_t> gateway;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // RRSIG(rfc4034, section 3.1)
        class RRSIG : public RR
        {
        public:
            RRSIG();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t type_covered = 0;
            std::uint8_t algorithm     = 0;
            std::uint8_t labels        = 0;
            std::uint32_t original     = 0;  // original ttl
            std::uint32_t expiration   = 0;  // signature expiration
            std::uint32_t inception    = 0;  // signature inception
            std::uint16_t key_tag      = 0;
            std::string signer;  // signer's name
            std::string signature;
        };


        // ---------------------------------------------------------------------
        // NSEC(rfc4034, section 4.1)
        class NSEC : public RR
        {
        public:
            NSEC();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string next_domain;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // DNSKEY(rfc4034, section 2.1)
        class DNSKEY : public RR
        {
        public:
            DNSKEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // DHCID(rfc4701, section 3.1)
        class DHCID : public RR
        {
        public:
            DHCID();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // NSEC3(rfc5155, section 3.2)
        class NSEC3 : public RR
        {
        public:
            NSEC3();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class NSEC3PARAM : public RR
        {
        public:
            NSEC3PARAM();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t hash        = 0;
            std::uint8_t flags       = 0;
            std::uint16_t iterations = 0;
            std::uint8_t salt_length = 0;
            std::vector<std::uint8_t> salt;
        };


        // ---------------------------------------------------------------------
        // TLSA(rfc6698, section 2.1)
        class TLSA : public RR
        {
        public:
            TLSA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t usage         = 0;
            std::uint8_t selector      = 0;
            std::uint8_t matching_type = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // SMIMEA(draft-ietf-dane-smime, section 2)
        class SMIMEA : public RR
        {
        public:
            SMIMEA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t usage         = 0;
            std::uint8_t selector      = 0;
            std::uint8_t matching_type = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // HIP(rfc5205, section 5)
        class HIP : public RR
        {
        public:
            HIP();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class NINFO : public RR
        {
        public:
            NINFO();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string zs_data;
        };


        // ---------------------------------------------------------------------
        // RKEY(draft-lewis-dns-undocumented-types-01)
        class RKEY : public RR
        {
        public:
            RKEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // TALINK(draft-lewis-dns-undocumented-types-01)
        class TALINK : public RR
        {
        public:
            TALINK();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string previous_name;
            std::string next_name;
        };


        // ---------------------------------------------------------------------
        // CDS(rfc7344, section 3.1)
        class CDS : public RR
        {
        public:
            CDS();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // CDNSKEY(rfc7344, section 3.2)
        class CDNSKEY : public RR
        {
        public:
            CDNSKEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // OPENPGPKEY(draft-ietf-dane-openpgpkey, section 2.1)
        class OPENPGPKEY : public RR
        {
        public:
            OPENPGPKEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // CSYNC(rfc7477, section 2.1.1)
        class CSYNC : public RR
        {
        public:
            CSYNC();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint32_t serial = 0;
            std::uint16_t flags  = 0;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // SPF(rfc4408, section 3)
        class SPF : public RR
        {
        public:
            SPF();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::string txt;
        };


        // ---------------------------------------------------------------------
        // UINFO(IANA reserved, no RFC documented)
        class UINFO : public Raw
        {
        public:
            UINFO();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // UID(IANA reserved, no RFC documented)
        class UID : public Raw
        {
        public:
            UID();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // GID(IANA reserved, no RFC documented)
        class GID : public Raw
        {
        public:
            GID();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // UNSPEC(IANA reserved, no RFC documented)
        class UNSPEC : public Raw
        {
        public:
            UNSPEC();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;
        };


        // ---------------------------------------------------------------------
        // NID(rfc6742, section 2.1.1)
        class NID : public RR
        {
        public:
            NID();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t preference = 0;
            std::uint64_t node_id    = 0;
        };


        // ---------------------------------------------------------------------
        // L32(rfc6742, section 2.2.1)
        class L32 : public RR
        {
        public:
            L32();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t preference = 0;
            std::uint32_t locator32  = 0;
        };


        // ---------------------------------------------------------------------
        // L64(rfc6742, section 2.3.1)
        class L64 : public RR
        {
        public:
            L64();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t preference = 0;
            std::uint32_t locator64  = 0;
        };


        // ---------------------------------------------------------------------
        // LP(rfc6742, section 2.4.1)
        class LP : public RR
        {
        public:
            LP();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t preference = 0;
            std::string fqdn;
        };


        // ---------------------------------------------------------------------
        // EUI48(rfc7043, section 3.1)
        class EUI48 : public RR
        {
        public:
            EUI48();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::array<std::uint8_t, 6> address;
        };


        // ---------------------------------------------------------------------
        // EUI64(rfc7043, section 4.1)
        class EUI64 : public RR
        {
        public:
            EUI64();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint64_t address = 0;
        };


        // ---------------------------------------------------------------------
        // TKEY(rfc2930, section 2)
        class TKEY : public RR
        {
        public:
            TKEY();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class TSIG : public RR
        {
        public:
            TSIG();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

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
        class URI : public RR
        {
        public:
            URI();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t priority = 0;
            std::uint16_t weight   = 0;
            std::string target;
        };


        // ---------------------------------------------------------------------
        // CAA(rfc6844, section 5.1.1)
        class CAA : public RR
        {
        public:
            CAA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint8_t flags = 0;
            std::string tag;
            std::string value;
        };


        // ---------------------------------------------------------------------
        // TA(Deploying DNSSEC Without a Signed Root)
        class TA : public RR
        {
        public:
            TA();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // DLV(rfc4431, section 2)
        class DLV : public RR
        {
        public:
            DLV();

        public:
            virtual std::shared_ptr<chen::dns::RR> clone() const override;

        protected:
            virtual void pack(std::vector<std::uint8_t> &out) const override;
            virtual void unpack(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };
    }
}