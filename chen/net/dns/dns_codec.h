/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_record.h"
#include <map>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // codec
        class codec
        {
        public:
            virtual ~codec() = default;

        public:
            /**
             * RR Pack & Unpack
             */
            void pack(const RR &rr, std::vector<std::uint8_t> &store);
            void unpack(RR &rr, const std::vector<std::uint8_t> &data);

        public:
            /**
             * Clear
             */
            void clear();

        public:
            /**
             * Data Pack & Unpack
             */
            static void pack(std::int8_t value, std::vector<std::uint8_t> &store);
            static void pack(std::int16_t value, std::vector<std::uint8_t> &store);
            static void pack(std::int32_t value, std::vector<std::uint8_t> &store);
            static void pack(std::uint8_t value, std::vector<std::uint8_t> &store);
            static void pack(std::uint16_t value, std::vector<std::uint8_t> &store);
            static void pack(std::uint32_t value, std::vector<std::uint8_t> &store);
            static void pack(chen::dns::RRType value, std::vector<std::uint8_t> &store);
            static void pack(chen::dns::RRClass value, std::vector<std::uint8_t> &store);
            static void pack(const std::string &value, bool domain, std::vector<std::uint8_t> &store);

        public:
            /**
             * Fully qualified domain name detection
             */
            static bool isFqdn(const std::string &name);
            static std::string fqdn(const std::string &name);

        private:
            std::map<std::string, std::size_t> _map;
        };
    }
}