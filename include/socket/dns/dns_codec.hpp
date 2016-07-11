/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_define.hpp"
#include "dns_error.hpp"
#include <chen/base/str.hpp>
#include <vector>
#include <array>

namespace chen
{
    namespace dns
    {
        // ---------------------------------------------------------------------
        // codec
        class codec
        {
        public:
            virtual ~codec() = 0;

        public:
            /**
             * Fully qualified domain name detection
             */
            static bool isFqdn(const std::string &name);
            static std::string& fqdn(std::string &name);
            static std::string fqdn(const std::string &name);
        };


        // ---------------------------------------------------------------------
        // encoder
        class encoder : public codec
        {
        public:
            /**
             * Pack data
             */
            static void pack(std::int8_t val, std::vector<std::uint8_t> &out);
            static void pack(std::int16_t val, std::vector<std::uint8_t> &out);
            static void pack(std::int32_t val, std::vector<std::uint8_t> &out);
            static void pack(std::int64_t val, std::vector<std::uint8_t> &out);
            static void pack(std::uint8_t val, std::vector<std::uint8_t> &out);
            static void pack(std::uint16_t val, std::vector<std::uint8_t> &out);
            static void pack(std::uint32_t val, std::vector<std::uint8_t> &out);
            static void pack(std::uint64_t val, std::vector<std::uint8_t> &out);
            static void pack(chen::dns::RRType val, std::vector<std::uint8_t> &out);
            static void pack(chen::dns::RRClass val, std::vector<std::uint8_t> &out);
            static void pack(const std::string &val, bool domain, std::vector<std::uint8_t> &out);
            static void pack(const std::vector<std::uint8_t> &val, std::size_t need, std::vector<std::uint8_t> &out);

            template <std::size_t Size>
            static void pack(const std::array<std::uint8_t, Size> &val, std::vector<std::uint8_t> &out)
            {
                out.insert(out.end(), val.begin(), val.end());
            }
        };


        // ---------------------------------------------------------------------
        // decoder
        class decoder : public codec
        {
        public:
            typedef std::vector<std::uint8_t>::const_iterator const_iterator;

        public:
            /**
             * Unpack data
             */
            static void unpack(std::int8_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::int16_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::int32_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::int64_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::uint8_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::uint16_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::uint32_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::uint64_t &val, const_iterator &cur, const_iterator &end);
            static void unpack(chen::dns::RRType &val, const_iterator &cur, const_iterator &end);
            static void unpack(chen::dns::RRClass &val, const_iterator &cur, const_iterator &end);
            static void unpack(std::string &val, bool domain, const_iterator &cur, const_iterator &end);
            static void unpack(std::vector<std::uint8_t> &val, std::size_t need, const_iterator &cur, const_iterator &end);

            template <std::size_t Size>
            static void unpack(std::array<std::uint8_t, Size> &val, const_iterator &cur, const_iterator &end)
            {
                auto need = val.size();
                if (end - cur < need)
                    throw error_size(str::format("dns: codec unpack array size is not enough, require %d bytes", need));

                std::copy(cur, cur + need, val.begin());
                cur += need;
            }
        };
    }
}