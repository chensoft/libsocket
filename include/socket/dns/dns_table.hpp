/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_define.hpp"
#include <memory>

namespace chen
{
    namespace dns
    {
        class RR;

        // todo put some convert methods back to dns_record
        class table
        {
        public:
            typedef std::shared_ptr<chen::dns::RR> rr_pointer;
            typedef rr_pointer (*rr_build_type)();

        public:
            /**
             * Build object
             */
            static rr_pointer build(chen::dns::RRType key);

        public:
            /**
             * Type & Text
             */
            static std::string typeToText(chen::dns::RRType key);
            static chen::dns::RRType textToType(const std::string &key);

            /**
             * Class & Text
             */
            static std::string classToText(chen::dns::RRClass key);
            static chen::dns::RRClass textToClass(const std::string &key);

            /**
             * QR & Text
             */
            static std::string qrToText(chen::dns::QR key);
            static chen::dns::QR textToQr(const std::string &key);

            /**
             * OPCODE & Text
             */
            static std::string opcodeToText(chen::dns::OPCODE key);
            static chen::dns::OPCODE textToOpcode(const std::string &key);

            /**
             * RCODE & Text
             */
            static std::string rcodeToText(chen::dns::RCODE key);
            static chen::dns::RCODE textToRcode(const std::string &key);

        public:
            /**
             * Set custom rr type and related value
             */
            static void set(chen::dns::RRType key, rr_build_type val);
            static void set(chen::dns::RRType key, const std::string &val);
            static void set(chen::dns::RRClass key, const std::string &val);
            static void set(chen::dns::QR key, const std::string &val);
            static void set(chen::dns::OPCODE key, const std::string &val);
            static void set(chen::dns::RCODE key, const std::string &val);
        };
    }
}