/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_record.h"
#include "dns_header.h"
#include "dns_packet.h"
#include <memory>
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
            /**
             * RR Pack & Unpack
             */
            static void pack(const chen::dns::RR &rr, std::vector<std::uint8_t> &store);
            static std::size_t unpack(std::shared_ptr<chen::dns::RR> &rr, const std::uint8_t *data, std::size_t size);

            /**
             * Header Pack & Unpack
             */
            static void pack(const chen::dns::header &header, std::vector<std::uint8_t> &store);
            static std::size_t unpack(chen::dns::header &header, const std::uint8_t *data, std::size_t size);

            /**
             * Question Pack & Unpack
             */
            static void pack(const chen::dns::question &question, std::vector<std::uint8_t> &store);
            static std::size_t unpack(chen::dns::question &question, const std::uint8_t *data, std::size_t size);

            /**
             * Request Pack & Unpack
             */
            static void pack(const chen::dns::request &request, std::vector<std::uint8_t> &store);
            static std::size_t unpack(chen::dns::request &request, const std::uint8_t *data, std::size_t size);

            /**
             * Response Pack & Unpack
             */
            static void pack(const chen::dns::response &response, std::vector<std::uint8_t> &store);
            static std::size_t unpack(chen::dns::response &response, const std::uint8_t *data, std::size_t size);

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
            static void pack(std::uint64_t value, std::vector<std::uint8_t> &store);
            static void pack(chen::dns::RRType value, std::vector<std::uint8_t> &store);
            static void pack(chen::dns::RRClass value, std::vector<std::uint8_t> &store);
            static void pack(const std::string &value, bool domain, std::vector<std::uint8_t> &store);
            // todo domain move to last param

            static std::size_t unpack(std::int8_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::int16_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::int32_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::uint8_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::uint16_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::uint32_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::uint64_t &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(chen::dns::RRType &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(chen::dns::RRClass &value, const std::uint8_t *data, std::size_t size);
            static std::size_t unpack(std::string &value, bool domain, const std::uint8_t *data, std::size_t size);

        private:
            /**
             * Check buffer size
             */
            static void check(std::size_t require, std::size_t size, const std::string &msg);

        public:
            /**
             * Fully qualified domain name detection
             */
            static bool isFqdn(const std::string &name);
            static std::string fqdn(const std::string &name);
        };
    }
}