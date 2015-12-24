/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_record.h"
#include <functional>
#include <memory>
#include <map>

namespace chen
{
    namespace dns
    {
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

        public:
            /**
             * Set custom rr type and related value
             */
            static void set(chen::dns::RRType key, rr_build_type val);
            static void set(chen::dns::RRType key, const std::string &val);
            static void set(chen::dns::RRClass key, const std::string &val);
            static void set(chen::dns::QR key, const std::string &val);
            static void set(chen::dns::OPCODE key, const std::string &val);

        private:
            /**
             * Init
             */
            static void init();

        private:
            static bool _init;

            static std::map<chen::dns::RRType, rr_build_type> _rr_build;

            static std::map<chen::dns::RRType, std::string> _rr_type_text;
            static std::map<std::string, chen::dns::RRType> _rr_text_type;

            static std::map<chen::dns::RRClass, std::string> _rr_class_text;
            static std::map<std::string, chen::dns::RRClass> _rr_text_class;

            static std::map<chen::dns::QR, std::string> _rr_qr_text;
            static std::map<std::string, chen::dns::QR> _rr_text_qr;

            static std::map<chen::dns::OPCODE, std::string> _rr_opcode_text;
            static std::map<std::string, chen::dns::OPCODE> _rr_text_opcode;
        };
    }
}