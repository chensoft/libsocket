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
             */
            virtual std::vector<std::uint8_t> encode() const;
            virtual void encode(std::vector<std::uint8_t> &out) const;

            virtual void decode(const std::vector<std::uint8_t> &data);
            virtual void decode(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end);

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
                             std::uint16_t id = header::random(),
                             bool recursionDesired = true);

        public:
            /**
             * Question
             */
            const chen::dns::question& question() const;
            chen::dns::question& question();

            void setQuestion(const chen::dns::question &value);
            void setQuestion(chen::dns::question &&value);

        public:
            /**
             * Client
             */
            const std::string& addr() const;
            std::uint16_t port() const;

            void setAddr(const std::string &addr);
            void setPort(std::uint16_t port);

        public:
            /**
             * Encode & Decode
             */
            virtual std::vector<std::uint8_t> encode() const override;
            virtual void encode(std::vector<std::uint8_t> &out) const override;

            using message::decode;

            virtual void decode(const std::vector<std::uint8_t> &data,
                                const std::string &addr,
                                std::uint16_t port);
            virtual void decode(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        protected:
            // client address info, empty if not set
            std::string _addr;
            std::uint16_t _port;

            // client question
            chen::dns::question _question;
        };


        // ---------------------------------------------------------------------
        // response
        class response : public message
        {
        public:
            typedef std::vector<chen::dns::question> q_type;
            typedef std::vector<std::shared_ptr<chen::dns::RR>> rr_type;

        public:
            explicit response(bool authoritative = false);

        public:
            /**
             * Add field value
             */
            void addQuestion(const chen::dns::question &value);
            void addQuestion(chen::dns::question &&value);

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
            void setQuestion(const chen::dns::request &value);

            void setQuestion(const q_type &value);
            void setAnswer(const rr_type &value);
            void setAuthority(const rr_type &value);
            void setAdditional(const rr_type &value);

            void setQuestion(q_type &&value);
            void setAnswer(rr_type &&value);
            void setAuthority(rr_type &&value);
            void setAdditional(rr_type &&value);

        public:
            /**
             * Rotate answers
             */
            void rotate();

        public:
            /**
             * Encode & Decode
             */
            virtual std::vector<std::uint8_t> encode() const override;
            virtual void encode(std::vector<std::uint8_t> &out) const override;

            using message::decode;

            virtual void decode(std::vector<std::uint8_t>::const_iterator &cur,
                                std::vector<std::uint8_t>::const_iterator &end) override;

        protected:
            q_type  _question;
            rr_type _answer;
            rr_type _authority;
            rr_type _additional;
        };
    }
}