/**
 * Created by Jian Chen
 * @since  2015.11.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_define.h"
#include <string>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // Resource Record
        struct RR
        {
        public:
            virtual ~RR() = default;

        public:
            std::string name;
            chen::dns::RRType rrtype   = chen::dns::RRType::None;
            chen::dns::RRClass rrclass = chen::dns::RRClass::IN;
            std::int32_t ttl = 0;

        protected:
            std::uint16_t rdlength = 0;
        };


        // ---------------------------------------------------------------------
        // A
        struct A : public RR
        {
        public:
            std::string address;  // IPv4 address
        };


        // ---------------------------------------------------------------------
        // NS
        struct NS : public RR
        {
        public:
            std::string nsdname;  // authoritative nameserver domain name
        };


        // ---------------------------------------------------------------------
        // CNAME
        struct CNAME : public RR
        {
        public:
            std::string cname;  // canonical name
        };


        // ---------------------------------------------------------------------
        // SOA
        struct SOA : public RR
        {
        public:
            std::string mname;  // primary nameserver
            std::string rname;  // hostmaster email address
            std::uint32_t serial = 0;  // serial number
            std::int32_t refresh = 0;  // zone refresh interval
            std::int32_t retry = 0;    // retry interval when refresh failed
            std::int32_t expire = 0;   // expire interval of the zone
        };


        // ---------------------------------------------------------------------
        // PTR
        struct PTR : public RR
        {
        public:
            std::string ptrdname;  // domain name which points to some location
        };


        // ---------------------------------------------------------------------
        // MX
        struct MX : public RR
        {
        public:
            std::int16_t preference = 0;  // preference of this record
            std::string exchange;  // a host act as a mail exchange
        };


        // ---------------------------------------------------------------------
        // TXT
        struct TXT : public RR
        {
        public:
            std::string txt_data;  // character strings
        };
    }
}