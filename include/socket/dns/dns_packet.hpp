/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_header.hpp"
#include "dns_record.hpp"
#include "dns_codec.hpp"

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // message
        class message
        {
        public:
            typedef chen::dns::header header_type;
            typedef chen::dns::question question_type;
            typedef std::shared_ptr<chen::dns::RR> record_type;

        public:
            /**
             * Property
             */
            header_type& header();

            const header_type& header() const;
            const std::vector<question_type>& question() const;
            const std::vector<record_type>& answer()     const;
            const std::vector<record_type>& authority()  const;
            const std::vector<record_type>& additional() const;

            void addQuestion(question_type value);
            void addAnswer(record_type value);
            void addAuthority(record_type value);
            void addAdditional(record_type value);

            void setQuestion(question_type value);
            void setAnswer(record_type value);
            void setAuthority(record_type value);
            void setAdditional(record_type value);

            void setQuestion(std::vector<question_type> value);
            void setAnswer(std::vector<record_type> value);
            void setAuthority(std::vector<record_type> value);
            void setAdditional(std::vector<record_type> value);

            /**
             * EDNS
             */
            std::shared_ptr<chen::dns::OPT> edns0() const;

            template <typename T>
            std::shared_ptr<T> option() const
            {
                auto edns0 = this->edns0();
                if (!edns0)
                    return nullptr;

                for (auto &ptr : edns0->options)
                {
                    auto ret = std::dynamic_pointer_cast<T>(ptr);
                    if (ret)
                        return ret;
                }

                return nullptr;
            }

        public:
            /**
             * Encode & Decode
             */
            void encode(chen::dns::encoder &encoder) const;
            void decode(chen::dns::decoder &decoder);

        protected:
            header_type _header;

            std::vector<question_type> _question;
            std::vector<record_type>   _answer;
            std::vector<record_type>   _authority;
            std::vector<record_type>   _additional;
        };


        // ---------------------------------------------------------------------
        // request
        class request : public message
        {
        public:
            request() = default;
            request(const std::string &qname, chen::dns::RRType qtype);

        public:
            /**
             * Query
             * usually only one question exist
             */
            const question_type& query() const;
            question_type& query();

            void setQuery(const std::string &qname, chen::dns::RRType qtype);

        public:
            /**
             * Client
             */
            std::string addr(bool subnet = false) const;
            std::uint16_t port() const;

            void setAddr(const std::string &addr);
            void setPort(std::uint16_t port);

        public:
            /**
             * Encode & Decode
             */
            using message::encode;
            using message::decode;

            std::vector<std::uint8_t> encode() const;
            void decode(chen::dns::codec::iterator beg, chen::dns::codec::iterator end,
                        const std::string &addr = "", std::uint16_t port = 0);

        protected:
            std::string _addr;  // client address info, empty if not set
            std::uint16_t _port;
        };


        // ---------------------------------------------------------------------
        // response
        class response : public message
        {
        public:
            response() = default;
            explicit response(bool authoritative);
            response(bool authoritative, const chen::dns::request &request);

        public:
            /**
             * Set question by request
             */
            using message::setQuestion;
            void setQuestion(const chen::dns::request &request);

            /**
             * Rotate answers
             */
            void rotate();

            /**
             * Make answers random
             */
            void random();

        public:
            /**
             * Encode & Decode
             */
            using message::encode;
            using message::decode;

            std::vector<std::uint8_t> encode() const;
            void decode(chen::dns::codec::iterator beg, chen::dns::codec::iterator end);
        };
    }
}