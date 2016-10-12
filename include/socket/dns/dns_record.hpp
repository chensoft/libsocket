/**
 * Created by Jian Chen
 * I'm very carefully to ensure that these rr's fields are accurate
 * Fields are whether signed or unsigned is according to its related rfc
 * Fields are little-endian, they will be convert to network byte order when encoding
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/dns/dns_define.hpp>
#include <socket/dns/dns_edns.hpp>
#include <chen/data/json.hpp>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // Resource Record(rfc1035, section 3.2.1)
        class rr
        {
        public:
            explicit rr(RRType type);
            virtual ~rr() = default;

        public:
            /**
             * Encode & Decode
             */
            void encode(dns::encoder &encoder) const;
            void decode(dns::decoder &decoder);
            void decode(const json::object &object);

            /**
             * Create rr and detect its type automatically
             */
            static std::shared_ptr<rr> create(dns::decoder &decoder);

        public:
            /**
             * Text description
             */
            virtual std::string str(const std::string &sep = " ") const = 0;

            /**
             * Clone current record
             */
            virtual std::shared_ptr<rr> clone() const = 0;

        protected:
            /**
             * Pack & Unpack
             */
            virtual void pack(dns::encoder &encoder) const = 0;
            virtual void unpack(dns::decoder &decoder) = 0;
            virtual void unpack(const json::object &object) = 0;

        public:
            /**
             * Check remain data when decode
             */
            std::size_t remain(std::size_t used) const;

            /**
             * Escape string, used in description
             */
            static std::string escape(const std::vector<std::uint8_t> &data);
            static std::string escape(const std::string &text);
            static std::string escape(std::size_t bits);

        protected:
            /**
             * Allow copy in derived class
             */
            rr(const rr&) = default;
            rr& operator=(const rr&) = default;

        public:
            std::string name;
            RRType rrtype    = TypeNone;
            RRClass rrclass  = ClassIN;
            std::int32_t ttl = 0;  // rfc1035 says it's a 32 bit signed integer

        protected:
            // internal use only, will be set when call unpack method
            std::uint16_t rdlength = 0;
        };


        // ---------------------------------------------------------------------
        // Custom rr
        // ---------------------------------------------------------------------
        // raw(raw resource record, with rdata)
        class raw : public rr
        {
        public:
            raw();
            explicit raw(RRType type);

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::vector<std::uint8_t> rdata;
        };


        // ---------------------------------------------------------------------
        // Standard rr
        // ---------------------------------------------------------------------
        // A(rfc1035, section 3.4.1)
        class rr_a : public rr
        {
        public:
            rr_a();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint32_t address = 0;
        };


        // ---------------------------------------------------------------------
        // NS(rfc1035, section 3.3.11)
        class rr_ns : public rr
        {
        public:
            rr_ns();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string nsdname;
        };


        // ---------------------------------------------------------------------
        // MD(rfc1035, section 3.3.4)
        class rr_md : public rr
        {
        public:
            rr_md();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // MF(rfc1035, section 3.3.5)
        class rr_mf : public rr
        {
        public:
            rr_mf();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // CNAME(rfc1035, section 3.3.1)
        class rr_cname : public rr
        {
        public:
            rr_cname();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string cname;
        };


        // ---------------------------------------------------------------------
        // SOA(rfc1035, section 3.3.13)
        class rr_soa : public rr
        {
        public:
            rr_soa();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_mb : public rr
        {
        public:
            rr_mb();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string madname;
        };


        // ---------------------------------------------------------------------
        // MG(rfc1035, section 3.3.6)
        class rr_mg : public rr
        {
        public:
            rr_mg();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string mgmname;
        };


        // ---------------------------------------------------------------------
        // MR(rfc1035, section 3.3.8)
        class rr_mr : public rr
        {
        public:
            rr_mr();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string newname;
        };


        // ---------------------------------------------------------------------
        // NULL(rfc1035, section 3.3.10, can store anything)
        class rr_null : public raw
        {
        public:
            rr_null();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::vector<std::uint8_t> anything;
        };


        // ---------------------------------------------------------------------
        // WKS(rfc1035, section 3.4.2)
        class rr_wks : public rr
        {
        public:
            rr_wks();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint32_t address = 0;
            std::uint8_t protocol = 0;
            std::vector<std::uint8_t> bitmap;
        };


        // ---------------------------------------------------------------------
        // PTR(rfc1035, section 3.3.12)
        class rr_ptr : public rr
        {
        public:
            rr_ptr();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string ptrdname;
        };


        // ---------------------------------------------------------------------
        // HINFO(rfc1035, section 3.3.2)
        class rr_hinfo : public rr
        {
        public:
            rr_hinfo();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string cpu;
            std::string os;
        };


        // ---------------------------------------------------------------------
        // MINFO(rfc1035, section 3.3.7)
        class rr_minfo : public rr
        {
        public:
            rr_minfo();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string rmailbx;
            std::string emailbx;
        };


        // ---------------------------------------------------------------------
        // MX(rfc1035, section 3.3.9)
        class rr_mx : public rr
        {
        public:
            rr_mx();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::int16_t preference = 0;
            std::string exchange;
        };


        // ---------------------------------------------------------------------
        // TXT(rfc1035, section 3.3.14)
        class rr_txt : public rr
        {
        public:
            rr_txt();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string txt_data;
        };


        // ---------------------------------------------------------------------
        // RP(rfc1183, section 2.2)
        class rr_rp : public rr
        {
        public:
            rr_rp();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string mbox_dname;
            std::string txt_dname;
        };


        // ---------------------------------------------------------------------
        // AFSDB(rfc1183, section 1)
        class rr_afsdb : public rr
        {
        public:
            rr_afsdb();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::int16_t subtype = 0;
            std::string hostname;
        };


        // ---------------------------------------------------------------------
        // X25(rfc1183, section 3.1)
        class rr_x25 : public rr
        {
        public:
            rr_x25();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string psdn_address;
        };


        // ---------------------------------------------------------------------
        // ISDN(rfc1183, section 3.2)
        class rr_isdn : public rr
        {
        public:
            rr_isdn();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string isdn_address;
            std::string sa;
        };


        // ---------------------------------------------------------------------
        // RT(rfc1183, section 3.3)
        class rr_rt : public rr
        {
        public:
            rr_rt();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::int16_t preference = 0;
            std::string intermediate_host;
        };


        // ---------------------------------------------------------------------
        // NSAP(rfc1706, section 5)
        class rr_nsap : public rr
        {
        public:
            rr_nsap();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string nsap;
        };


        // ---------------------------------------------------------------------
        // NSAPPTR(rfc1348)
        class rr_nsapptr : public rr
        {
        public:
            rr_nsapptr();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string owner;
        };


        // ---------------------------------------------------------------------
        // SIG(rfc2535, section 4.1)
        class rr_sig : public rr
        {
        public:
            rr_sig();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_key : public rr
        {
        public:
            rr_key();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // PX(rfc2163, section 4)
        class rr_px : public rr
        {
        public:
            rr_px();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::int16_t preference = 0;
            std::string map822;
            std::string mapx400;
        };


        // ---------------------------------------------------------------------
        // GPOS(rfc1712, section 3)
        class rr_gpos : public rr
        {
        public:
            rr_gpos();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string longitude;
            std::string latitude;
            std::string altitude;
        };


        // ---------------------------------------------------------------------
        // AAAA(rfc3596, section 2.2)
        class rr_aaaa : public rr
        {
        public:
            rr_aaaa();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::array<std::uint8_t, 16> address = {};
        };


        // ---------------------------------------------------------------------
        // LOC(rfc1876, section 2)
        class rr_loc : public rr
        {
        public:
            rr_loc();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_nxt : public rr
        {
        public:
            rr_nxt();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string next_domain;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // EID(draft-lewis-dns-undocumented-types-01)
        class rr_eid : public rr
        {
        public:
            rr_eid();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string endpoint;
        };


        // ---------------------------------------------------------------------
        // NIMLOC(draft-lewis-dns-undocumented-types-01)
        class rr_nimloc : public rr
        {
        public:
            rr_nimloc();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string locator;
        };


        // ---------------------------------------------------------------------
        // SRV(rfc2782)
        class rr_srv : public rr
        {
        public:
            rr_srv();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t priority = 0;
            std::uint16_t weight   = 0;
            std::uint16_t port     = 0;
            std::string target;
        };


        // ---------------------------------------------------------------------
        // ATMA(ATM Name System Specification Version 1.0)
        class rr_atma : public rr
        {
        public:
            rr_atma();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t format = 0;
            std::string address;
        };


        // ---------------------------------------------------------------------
        // NAPTR(rfc3403, section 4.1)
        class rr_naptr : public rr
        {
        public:
            rr_naptr();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_kx : public rr
        {
        public:
            rr_kx();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t preference = 0;
            std::string exchanger;
        };


        // ---------------------------------------------------------------------
        // CERT(rfc4398, section 2)
        class rr_cert : public rr
        {
        public:
            rr_cert();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t type     = 0;
            std::uint16_t key_tag  = 0;
            std::uint8_t algorithm = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // A6(rfc2874, section 3.1, OBSOLETE - use AAAA instead)
        class rr_a6 : public rr
        {
        public:
            rr_a6();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t prefix = 0;
            std::vector<std::uint8_t> suffix;
            std::string prefix_name;
        };


        // ---------------------------------------------------------------------
        // DNAME(rfc2672, section 3)
        class rr_dname : public rr
        {
        public:
            rr_dname();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string target;
        };


        // ---------------------------------------------------------------------
        // SINK(draft-eastlake-kitchen-sink-02)
        class rr_sink : public rr
        {
        public:
            rr_sink();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t coding    = 0;
            std::uint8_t subcoding = 0;
            std::vector<std::uint8_t> sdata;
        };


        // ---------------------------------------------------------------------
        // OPT(rfc6891, section 6.1.2)
        class rr_opt : public rr
        {
        public:
            typedef std::shared_ptr<edns0::opt> opt_pointer;

        public:
            rr_opt();

        public:
            /**
             * Requestor's UDP payload size, stores in the rrclass field
             * @see rfc6891, section 6.1.2
             */
            std::uint16_t payload() const;

            /**
             * Extended RCODE and flags, stores in the ttl field
             * @see rfc6891, section 6.1.3 & 6.1.4
             */
            std::uint16_t rcode(RCode code) const;  // upper 8 bits combine with 4 bits original rcode
            std::uint8_t rcode() const;             // upper 8 bits of extended 12-bit RCODE
            std::uint8_t version() const;           // edns version

            std::uint16_t flag() const;  // edns flags
            bool dnssec() const;         // DNSSEC OK bit
            std::uint16_t z() const;     // reserved zero bits

        public:
            /**
             * Set property value
             */
            void payload(std::uint16_t value);

            void rcode(std::uint16_t value);   // full 12 bits value
            void rcode(std::uint8_t value);    // upper 8 bits
            void version(std::uint8_t value);  // edns version

            void flag(std::uint16_t value);  // edns flags
            void dnssec(bool value);         // DNSSEC OK bit
            void z(std::uint16_t value);     // reserved zero bits

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::vector<opt_pointer> options;
        };


        // ---------------------------------------------------------------------
        // DS(rfc4034, section 5.1)
        class rr_ds : public rr
        {
        public:
            rr_ds();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // SSHFP(rfc4255, section 3.1)
        class rr_sshfp : public rr
        {
        public:
            rr_sshfp();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t algorithm = 0;
            std::uint8_t fptype    = 0;
            std::string fingerprint;
        };


        // ---------------------------------------------------------------------
        // IPSECKEY(rfc4025, section 2.1)
        class rr_ipseckey : public rr
        {
        public:
            enum class GatewayType {None, IPv4, IPv6, Domain};

        public:
            rr_ipseckey();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t precedence   = 0;
            std::uint8_t gateway_type = 0;  // 0: no type, gateway should be '.', 1: IPv4, 4 bytes, 2: IPv6, 16 bytes, 3: domain name
            std::uint8_t algorithm    = 0;
            std::vector<std::uint8_t> gateway;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // RRSIG(rfc4034, section 3.1)
        class rr_rrsig : public rr
        {
        public:
            rr_rrsig();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_nsec : public rr
        {
        public:
            rr_nsec();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string next_domain;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // DNSKEY(rfc4034, section 2.1)
        class rr_dnskey : public rr
        {
        public:
            rr_dnskey();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // DHCID(rfc4701, section 3.1)
        class rr_dhcid : public rr
        {
        public:
            rr_dhcid();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // NSEC3(rfc5155, section 3.2)
        class rr_nsec3 : public rr
        {
        public:
            rr_nsec3();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_nsec3param : public rr
        {
        public:
            rr_nsec3param();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t hash        = 0;
            std::uint8_t flags       = 0;
            std::uint16_t iterations = 0;
            std::uint8_t salt_length = 0;
            std::vector<std::uint8_t> salt;
        };


        // ---------------------------------------------------------------------
        // TLSA(rfc6698, section 2.1)
        class rr_tlsa : public rr
        {
        public:
            rr_tlsa();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t usage         = 0;
            std::uint8_t selector      = 0;
            std::uint8_t matching_type = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // SMIMEA(draft-ietf-dane-smime, section 2)
        class rr_smimea : public rr
        {
        public:
            rr_smimea();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t usage         = 0;
            std::uint8_t selector      = 0;
            std::uint8_t matching_type = 0;
            std::string certificate;
        };


        // ---------------------------------------------------------------------
        // HIP(rfc5205, section 5)
        class rr_hip : public rr
        {
        public:
            rr_hip();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_ninfo : public rr
        {
        public:
            rr_ninfo();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string zs_data;
        };


        // ---------------------------------------------------------------------
        // RKEY(draft-lewis-dns-undocumented-types-01)
        class rr_rkey : public rr
        {
        public:
            rr_rkey();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // TALINK(draft-lewis-dns-undocumented-types-01)
        class rr_talink : public rr
        {
        public:
            rr_talink();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string previous_name;
            std::string next_name;
        };


        // ---------------------------------------------------------------------
        // CDS(rfc7344, section 3.1)
        class rr_cds : public rr
        {
        public:
            rr_cds();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // CDNSKEY(rfc7344, section 3.2)
        class rr_cdnskey : public rr
        {
        public:
            rr_cdnskey();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t flags    = 0;
            std::uint8_t protocol  = 0;
            std::uint8_t algorithm = 0;
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // OPENPGPKEY(draft-ietf-dane-openpgpkey, section 2.1)
        class rr_openpgpkey : public rr
        {
        public:
            rr_openpgpkey();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string publickey;
        };


        // ---------------------------------------------------------------------
        // CSYNC(rfc7477, section 2.1.1)
        class rr_csync : public rr
        {
        public:
            rr_csync();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint32_t serial = 0;
            std::uint16_t flags  = 0;
            std::vector<std::uint8_t> type_bitmap;
        };


        // ---------------------------------------------------------------------
        // SPF(rfc4408, section 3)
        class rr_spf : public rr
        {
        public:
            rr_spf();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string txt;
        };


        // ---------------------------------------------------------------------
        // UINFO(IANA reserved, no RFC documented)
        class rr_uinfo : public raw
        {
        public:
            rr_uinfo();

        public:
            virtual std::shared_ptr<rr> clone() const override;
        };


        // ---------------------------------------------------------------------
        // UID(IANA reserved, no RFC documented)
        class rr_uid : public raw
        {
        public:
            rr_uid();

        public:
            virtual std::shared_ptr<rr> clone() const override;
        };


        // ---------------------------------------------------------------------
        // GID(IANA reserved, no RFC documented)
        class rr_gid : public raw
        {
        public:
            rr_gid();

        public:
            virtual std::shared_ptr<rr> clone() const override;
        };


        // ---------------------------------------------------------------------
        // UNSPEC(IANA reserved, no RFC documented)
        class rr_unspec : public raw
        {
        public:
            rr_unspec();

        public:
            virtual std::shared_ptr<rr> clone() const override;
        };


        // ---------------------------------------------------------------------
        // NID(rfc6742, section 2.1.1)
        class rr_nid : public rr
        {
        public:
            rr_nid();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t preference = 0;
            std::uint64_t node_id    = 0;
        };


        // ---------------------------------------------------------------------
        // L32(rfc6742, section 2.2.1)
        class rr_l32 : public rr
        {
        public:
            rr_l32();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t preference = 0;
            std::uint32_t locator32  = 0;
        };


        // ---------------------------------------------------------------------
        // L64(rfc6742, section 2.3.1)
        class rr_l64 : public rr
        {
        public:
            rr_l64();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t preference = 0;
            std::uint32_t locator64  = 0;
        };


        // ---------------------------------------------------------------------
        // LP(rfc6742, section 2.4.1)
        class rr_lp : public rr
        {
        public:
            rr_lp();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t preference = 0;
            std::string fqdn;
        };


        // ---------------------------------------------------------------------
        // EUI48(rfc7043, section 3.1)
        class rr_eui48 : public rr
        {
        public:
            rr_eui48();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::array<std::uint8_t, 6> address = {};
        };


        // ---------------------------------------------------------------------
        // EUI64(rfc7043, section 4.1)
        class rr_eui64 : public rr
        {
        public:
            rr_eui64();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint64_t address = 0;
        };


        // ---------------------------------------------------------------------
        // TKEY(rfc2930, section 2)
        class rr_tkey : public rr
        {
        public:
            rr_tkey();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

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
        class rr_tsig : public rr
        {
        public:
            rr_tsig();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::string algorithm;
            std::array<std::uint8_t, 6> time_signed = {};
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
        class rr_uri : public rr
        {
        public:
            rr_uri();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t priority = 0;
            std::uint16_t weight   = 0;
            std::string target;
        };


        // ---------------------------------------------------------------------
        // CAA(rfc6844, section 5.1.1)
        class rr_caa : public rr
        {
        public:
            rr_caa();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint8_t flags = 0;
            std::string tag;
            std::string value;
        };


        // ---------------------------------------------------------------------
        // TA(Deploying DNSSEC Without a Signed Root)
        class rr_ta : public rr
        {
        public:
            rr_ta();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };


        // ---------------------------------------------------------------------
        // DLV(rfc4431, section 2)
        class rr_dlv : public rr
        {
        public:
            rr_dlv();

        public:
            virtual std::string str(const std::string &sep = " ") const override;
            virtual std::shared_ptr<rr> clone() const override;

        protected:
            virtual void pack(dns::encoder &encoder) const override;
            virtual void unpack(dns::decoder &decoder) override;
            virtual void unpack(const json::object &object) override;

        public:
            std::uint16_t key_tag    = 0;
            std::uint8_t algorithm   = 0;
            std::uint8_t digest_type = 0;
            std::string digest;
        };
    }
}