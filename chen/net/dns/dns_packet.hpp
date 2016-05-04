/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_header.hpp"
#include <vector>
#include <memory>

namespace chen
{
    namespace dns
    {
        class RR;

        // ---------------------------------------------------------------------
        // message
        class message
        {
        public:
            virtual ~message() = 0;
        };


        // ---------------------------------------------------------------------
        // request
        class request : public message
        {
        public:
            /**
             * Set question
             */
            void setQuestion(const std::string &qname,
                             chen::dns::RRType qtype,
                             chen::dns::RRClass qclass = chen::dns::RRClass::IN,
                             std::uint16_t id = 0,
                             bool recursionDesired = true);

        public:
            /**
             * Get field value
             */
            const chen::dns::header& header()     const;
            const chen::dns::question& question() const;

            /**
             * Get filed and modify it
             * usually you don't need to do so
             */
            chen::dns::header& header();
            chen::dns::question& question();

            /**
             * Set field value
             */
            void setHeader(const chen::dns::header &value);
            void setQuestion(const chen::dns::question &value);

        public:
            /**
             * Get client address and port
             */
            std::string addr()   const;
            std::uint16_t port() const;

            /**
             * Set client address and port
             */
            void setAddr(const std::string &value);
            void setPort(std::uint16_t value);

        public:
            /**
             * Encode & Decode
             * todo return remain bytes or use iterator when use decode?
             */
            std::vector<std::uint8_t> encode() const;
            void decode(const std::vector<std::uint8_t> &data);

        private:
            chen::dns::header _header;
            chen::dns::question _question;

            // client information when receive request
            std::string _addr;
            std::uint16_t _port = 0;
        };


        // ---------------------------------------------------------------------
        // response
        class response : public message
        {
        public:
            typedef std::vector<chen::dns::question> q_type;
            typedef std::vector<std::shared_ptr<chen::dns::RR>> rr_type;

        public:
            response() = default;
            ~response() = default;

            response(const response &o);
            response& operator=(const response &o);

            response(response &&o);
            response& operator=(response &&o);

        public:
            /**
             * Get field value
             */
            const chen::dns::header& header() const;
            const q_type& question()          const;
            const rr_type& answer()           const;
            const rr_type& authority()        const;
            const rr_type& additional()       const;

        public:
            /**
             * Add field value
             */
            void addQuestion(const chen::dns::question &value);
            void addAnswer(std::shared_ptr<chen::dns::RR> value);
            void addAuthority(std::shared_ptr<chen::dns::RR> value);
            void addAdditional(std::shared_ptr<chen::dns::RR> value);

        public:
            /**
             * Set field value
             */
            void setHeader(const chen::dns::header &value);
            void setQuestion(const q_type &value);
            void setAnswer(const rr_type &value);
            void setAuthority(const rr_type &value);
            void setAdditional(const rr_type &value);

        public:
            /**
             * Encode & Decode
             * todo return remain bytes or use iterator when use decode?
             */
            std::vector<std::uint8_t> encode() const;
            void decode(const std::vector<std::uint8_t> &data);

        private:
            chen::dns::header _header;

            q_type  _question;
            rr_type _answer;
            rr_type _authority;
            rr_type _additional;
        };
    }
}