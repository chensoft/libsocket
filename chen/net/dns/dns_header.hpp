/**
 * Created by Jian Chen
 * @since  2015.11.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_define.hpp"
#include <string>

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
            std::int16_t  id()      const;  // identifier
            std::uint16_t flag()    const;  // dns flags
            std::uint16_t qdcount() const;  // question count
            std::uint16_t ancount() const;  // answer count
            std::uint16_t nscount() const;  // nameserver count
            std::uint16_t arcount() const;  // additional count

            /**
             * Get flag value
             */
            chen::dns::QR qr()         const;  // query or response
            chen::dns::OPCODE opcode() const;  // operation code
            bool authoritative()       const;  // authoritative answer
            bool truncated()           const;  // truncated response
            bool recursionDesired()    const;  // recursion desired
            bool recursionAvailable()  const;  // recursion available
            bool zero()                const;  // reserved
            bool authenticData()       const;  // authentic data
            bool checkingDisabled()    const;  // checking disabled
            chen::dns::RCODE rcode()   const;  // response code

            /**
             * Set filed value
             */
            void setId(std::int16_t value = 0);  // use random id when value is zero
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
            void setAuthoritative(bool value);
            void setTruncated(bool value);
            void setRecursionDesired(bool value);
            void setRecursionAvailable(bool value);
            void setZero(bool value);
            void setAuthenticData(bool value);
            void setCheckingDisabled(bool value);
            void setRcode(chen::dns::RCODE value);

        public:
            /**
             * Random identifier
             */
            static std::int16_t random();

        private:
            std::int16_t  _id      = 0;  // the identifier of a query
            std::uint16_t _flag    = 0;  // QR, OPCODE, AA, TC, RD, RA, Z, RCODE
            std::uint16_t _qdcount = 0;  // question count, always be 1 in query
            std::uint16_t _ancount = 0;  // the number of resource records in the answer section
            std::uint16_t _nscount = 0;  // the number of name server resource records in the authority records section
            std::uint16_t _arcount = 0;  // the number of resource records in the additional records section
        };


        // ---------------------------------------------------------------------
        // question
        class question
        {
        public:
            /**
             * Get field value
             */
            std::string qname()         const;
            chen::dns::RRType qtype()   const;
            chen::dns::RRClass qclass() const;

            /**
             * Set field value
             */
            void setQname(const std::string &value);
            void setQtype(chen::dns::RRType value);
            void setQclass(chen::dns::RRClass value);

        private:
            std::string _qname;
            chen::dns::RRType  _qtype  = chen::dns::RRType::None;
            chen::dns::RRClass _qclass = chen::dns::RRClass::IN;
        };
    }
}