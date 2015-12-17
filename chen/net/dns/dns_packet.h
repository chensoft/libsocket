/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_header.h"
#include "dns_record.h"
#include <vector>

namespace chen
{
    namespace dns
    {
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
            request();

        public:
            /**
             * Get field value
             */
            const chen::dns::header& header()     const;
            const chen::dns::question& question() const;

        public:
            /**
             * Set query data
             */
            void setQuery(const std::string &qname,
                          chen::dns::RRType qtype,
                          chen::dns::RRClass qclass = chen::dns::RRClass::IN);

            /**
             * Enable RD flag, default is true
             */
            void setRecursionDesired(bool value);

        private:
            chen::dns::header _header;
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
            /**
             * Get field value
             */
            const chen::dns::header& header();
            const q_type& question();
            const rr_type& answer();
            const rr_type& authority();
            const rr_type& additional();

        public:
            /**
             * Set field value
             */
            void setHeader(const chen::dns::header &value);
            void setQuestion(const q_type &value);
            void setAnswer(const rr_type &value);
            void setAuthority(const rr_type &value);
            void setAdditional(const rr_type &value);

        private:
            chen::dns::header _header;

            q_type  _question;
            rr_type _answer;
            rr_type _authority;
            rr_type _additional;
        };
    }
}