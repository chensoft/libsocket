/**
 * Created by Jian Chen
 * @since  2015.11.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/prot/dns/dns_define.hpp>
#include <string>

namespace chen
{
    namespace dns
    {
        class encoder;
        class decoder;

        // ---------------------------------------------------------------------
        // header
        class header
        {
        public:
            /**
             * Get filed value
             */
            std::uint16_t id()      const;  // identifier
            std::uint16_t flag()    const;  // dns flags
            std::uint16_t qdcount() const;  // question count
            std::uint16_t ancount() const;  // answer count
            std::uint16_t nscount() const;  // nameserver count
            std::uint16_t arcount() const;  // additional count

            /**
             * Set filed value
             */
            void id(std::uint16_t value);
            void flag(std::uint16_t value);
            void qdcount(std::uint16_t value);
            void ancount(std::uint16_t value);
            void nscount(std::uint16_t value);
            void arcount(std::uint16_t value);

        public:
            /**
             * Get flag value
             */
            QrCode qr()               const;  // query or response
            OpCode opcode()           const;  // operation code
            bool authoritative()      const;  // authoritative answer
            bool truncated()          const;  // truncated response
            bool recursionDesired()   const;  // recursion desired
            bool recursionAvailable() const;  // recursion available
            bool zero()               const;  // reserved
            bool authenticData()      const;  // authentic data
            bool checkingDisabled()   const;  // checking disabled
            RCode rcode()             const;  // response code

            /**
             * Set flag value
             */
            void qr(QrCode value);
            void opcode(OpCode value);
            void authoritative(bool value);
            void truncated(bool value);
            void recursionDesired(bool value);
            void recursionAvailable(bool value);
            void zero(bool value);
            void authenticData(bool value);
            void checkingDisabled(bool value);
            void rcode(RCode value);

        public:
            /**
             * Encode & Decode
             */
            void encode(dns::encoder &encoder) const;
            void decode(dns::decoder &decoder);

        public:
            /**
             * Random identifier
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
        // question
        class question
        {
        public:
            question() = default;
            question(const std::string &qname, RRType qtype, RRClass qclass = ClassIN);

        public:
            /**
             * Get field value
             */
            const std::string& qname()  const;
            RRType qtype()   const;
            RRClass qclass() const;

            /**
             * Set field value
             */
            void qname(const std::string &value);
            void qname(std::string &&value);
            void qtype(RRType value);
            void qclass(RRClass value);

        public:
            /**
             * Encode & Decode
             */
            void encode(dns::encoder &encoder) const;
            void decode(dns::decoder &decoder);

        public:
            /**
             * Text description
             */
            std::string str(const std::string &sep = " ") const;

        private:
            std::string _qname;
            RRType  _qtype  = TypeNone;
            RRClass _qclass = ClassIN;
        };
    }
}