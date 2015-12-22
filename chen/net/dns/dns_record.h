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
    }
}