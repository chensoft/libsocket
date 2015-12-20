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

            // rdlength, uint16_t
        };


        // ---------------------------------------------------------------------
        // Unknown
        struct Unknown : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

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
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::int32_t address;  // IPv4 address
        };


        // ---------------------------------------------------------------------
        // NS(rfc1035, section 3.3.11)
        struct NS : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::string nsdname;  // authoritative nameserver domain name
        };


        // ---------------------------------------------------------------------
        // CNAME(rfc1035, section 3.3.1)
        struct CNAME : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::string cname;  // canonical name
        };


        // ---------------------------------------------------------------------
        // SOA(rfc1035, section 3.3.13)
        struct SOA : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::string mname;  // primary nameserver
            std::string rname;  // hostmaster email address
            std::uint32_t serial = 0;  // serial number
            std::int32_t refresh = 0;  // zone refresh interval
            std::int32_t retry   = 0;  // retry interval when refresh failed
            std::int32_t expire  = 0;  // expire interval of the zone
        };


        // ---------------------------------------------------------------------
        // PTR(rfc1035, section 3.3.12)
        struct PTR : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::string ptrdname;  // domain name which points to some location
        };


        // ---------------------------------------------------------------------
        // MX(rfc1035, section 3.3.9)
        struct MX : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::int16_t preference = 0;  // preference of this record
            std::string exchange;  // a host act as a mail exchange
        };


        // ---------------------------------------------------------------------
        // TXT(rfc1035, section 3.3.14)
        struct TXT : public RR
        {
        public:
            /**
             * Data
             */
            virtual std::vector<std::uint8_t> data() const;
            virtual std::size_t setData(const std::uint8_t *data, std::size_t size);

        public:
            std::string txt_data;  // character strings
        };
    }
}