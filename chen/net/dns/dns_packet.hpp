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

        public:
            /**
             * Get field value
             */
            const chen::dns::header& header() const;
            chen::dns::header& header();

            /**
             * Set field value
             */
            void setHeader(const chen::dns::header &value);

        public:
            /**
             * Encode & Decode
             * todo return remain bytes or use iterator when use decode?
             */
            virtual std::vector<std::uint8_t> encode() const;
            virtual void decode(const std::vector<std::uint8_t> &data);

        protected:
            chen::dns::header _header;
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
            const chen::dns::question& question() const;
            chen::dns::question& question();

            /**
             * Set field value
             */
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
            virtual std::vector<std::uint8_t> encode() const override;
            virtual void decode(const std::vector<std::uint8_t> &data) override;

        protected:
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
            response();

            response(const response &o);
            response& operator=(const response &o);

            response(response &&o);
            response& operator=(response &&o);

        public:
            /**
             * Add field value
             */
            void addQuestion(const chen::dns::request &value);
            void addQuestion(const chen::dns::question &value);
            void addAnswer(std::shared_ptr<chen::dns::RR> value);
            void addAuthority(std::shared_ptr<chen::dns::RR> value);
            void addAdditional(std::shared_ptr<chen::dns::RR> value);

        public:
            /**
             * Get field value
             */
            const q_type& question()    const;
            const rr_type& answer()     const;
            const rr_type& authority()  const;
            const rr_type& additional() const;

        public:
            /**
             * Set field value
             */
            void setQuestion(const q_type &value);
            void setAnswer(const rr_type &value);
            void setAuthority(const rr_type &value);
            void setAdditional(const rr_type &value);

        public:
            /**
             * Encode & Decode
             * todo return remain bytes or use iterator when use decode?
             */
            virtual std::vector<std::uint8_t> encode() const override;
            virtual void decode(const std::vector<std::uint8_t> &data) override;

        protected:
            q_type  _question;
            rr_type _answer;
            rr_type _authority;
            rr_type _additional;
        };
    }
}