/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * @link   https://tools.ietf.org/html/rfc1035
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "dns_type.h"

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // Resource Record
        class RR
        {
        public:
            /**
             * Get filed value
             */
            std::string name()           const;
            chen::dns::RRType rrtype()   const;
            chen::dns::RRClass rrclass() const;
            std::int32_t ttl()       const;
            std::uint16_t rdlength() const;
            std::vector<std::uint8_t> rdata() const;

            /**
             * Set filed value
             */
            void setName(const std::string &value);
            void setRrtype(chen::dns::RRType value);
            void setRrclass(chen::dns::RRClass value);
            void setTtl(std::int32_t value);
            void setRdlength(std::uint16_t value);
            void setRdata(const std::uint8_t *data, std::size_t size);

        public:
            /**
             * Assign binary data to RR
             */
            std::size_t assign(const std::uint8_t *cur_data, std::size_t cur_size,
                               const std::uint8_t *org_data, std::size_t org_size);

        private:
            std::string _name;
            chen::dns::RRType _rrtype   = chen::dns::RRType::None;
            chen::dns::RRClass _rrclass = chen::dns::RRClass::IN;
            std::int32_t _ttl       = 0;
            std::uint16_t _rdlength = 0;

            std::vector<std::uint8_t> _rdata;
        };


        // ---------------------------------------------------------------------
        // A
        class A : public RR
        {
        public:

        private:
            std::string _address;
        };
    }
}