/**
 * Created by Jian Chen
 * @since  2015.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/dns/dns_define.hpp>
#include <socket/dns/dns_record.hpp>

namespace chen
{
    namespace dns
    {
        class table
        {
        public:
            typedef std::shared_ptr<RR> rr_pointer;
            typedef rr_pointer (*rr_build_type)();

            typedef std::shared_ptr<edns0::opt> opt_pointer;
            typedef opt_pointer (*opt_build_type)();

        public:
            /**
             * Build object
             */
            static rr_pointer build(RRType key);
            static opt_pointer build(edns0::OptCode key);

        public:
            /**
             * Type & Text
             */
            static std::string typeToText(RRType key);
            static RRType textToType(const std::string &key);

            /**
             * Class & Text
             */
            static std::string classToText(RRClass key);
            static RRClass textToClass(const std::string &key);

            /**
             * QR & Text
             */
            static std::string qrToText(QrCode key);
            static QrCode textToQr(const std::string &key);

            /**
             * OPCODE & Text
             */
            static std::string opcodeToText(OpCode key);
            static OpCode textToOpcode(const std::string &key);

            /**
             * RCODE & Text
             */
            static std::string rcodeToText(RCode key);
            static RCode textToRcode(const std::string &key);

            /**
             * EDNS0 OptionCode & Text
             */
            static std::string edns0ToText(edns0::OptCode key);
            static edns0::OptCode textToEDNS0(const std::string &key);

        public:
            /**
             * Set custom rr type and related value
             */
            static void set(RRType key, rr_build_type val);
            static void set(edns0::OptCode key, opt_build_type val);
            static void set(RRType key, const std::string &val);
            static void set(RRClass key, const std::string &val);
            static void set(QrCode key, const std::string &val);
            static void set(OpCode key, const std::string &val);
            static void set(RCode key, const std::string &val);
            static void set(edns0::OptCode key, const std::string &val);
        };
    }
}