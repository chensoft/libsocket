/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * @link   https://tools.ietf.org/html/rfc1035
 */
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "dns_type.h"

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // header
        class header
        {
        public:
            /**
             * Get filed value
             */
            std::uint16_t id()      const;
            std::uint16_t flag()    const;
            std::uint16_t qdcount() const;
            std::uint16_t ancount() const;
            std::uint16_t nscount() const;
            std::uint16_t arcount() const;

            /**
             * Get flag value
             */
            chen::dns::QR qr()         const;
            chen::dns::OPCODE opcode() const;
            chen::dns::AA aa()         const;
            chen::dns::TC tc()         const;
            chen::dns::RD rd()         const;
            chen::dns::RA ra()         const;
            chen::dns::Z z()           const;
            chen::dns::RCODE rcode()   const;

            /**
             * Set filed value
             */
            void setId(std::uint16_t value = 0);  // if it's zero then will use random id
            void setFlag(std::uint16_t value);
            void setQdcount(std::uint16_t value);
            void setAncount(std::uint16_t value);
            void setNscount(std::uint16_t value);
            void setArcount(std::uint16_t value);

            /**
             * Set flag value
             */
            void setQr(chen::dns::QR value);
            void setOpcode(chen::dns::OPCODE value);
            void setAa(chen::dns::AA value);
            void setTc(chen::dns::TC value);
            void setRd(chen::dns::RD value);
            void setRa(chen::dns::RA value);
            void setZ(chen::dns::Z value);
            void setRcode(chen::dns::RCODE value);

            /**
             * Binary data of header
             */
            std::vector<std::uint8_t> binary() const;
            void binary(std::vector<std::uint8_t> &store) const;

            /**
             * Random int
             */
            static std::uint16_t random();

        private:
            std::uint16_t _id      = 0;  // the identifier of a query
            std::uint16_t _flag    = 0;  // QR, OPCODE, AA, TC, RD, RA, Z, RCODE
            std::uint16_t _qdcount = 0;  // question count, always be 1 in query
            std::uint16_t _ancount = 0;  // the number of resource records in the answer section
            std::uint16_t _nscount = 0;  // the number of name server resource records in the authority records section
            std::uint16_t _arcount = 0;  // the number of resource records in the additional records section
        };


        // ---------------------------------------------------------------------
        // packet
        class packet
        {
        public:
            /**
             * Pack domain name to binary
             */
            static void packDomain(const std::string &name, std::vector<std::uint8_t> &store);
        };


        // ---------------------------------------------------------------------
        // question
        class question : public packet
        {
        public:
            question();

        public:
            /**
             * Get field value
             */
            header qheader()            const;
            std::string qname()         const;
            chen::dns::RRType qtype()   const;
            chen::dns::RRClass qclass() const;

            /**
             * Set query data
             */
            void setQuery(const std::string &qname,
                          chen::dns::RRType qtype,
                          chen::dns::RRClass qclass = chen::dns::RRClass::IN);

            /**
             * Enable RD flag, default is true
             */
            void setRecursionDesired();

            /**
             * Binary data of question
             */
            std::vector<std::uint8_t> binary() const;
            void binary(std::vector<std::uint8_t> &store) const;

        private:
            header _qheader;

            std::string _qname;
            chen::dns::RRType  _qtype  = chen::dns::RRType::None;
            chen::dns::RRClass _qclass = chen::dns::RRClass::IN;
        };


        // ---------------------------------------------------------------------
        // response
        class response : public packet
        {
        public:

        private:
            header _header;
        };
    }
}