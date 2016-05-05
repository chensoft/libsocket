/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_define.hpp"
#include "dns_error.hpp"
#include <chen/tool/str.hpp>
#include <memory>
#include <vector>
#include <array>

namespace chen
{
    namespace dns
    {
        class RR;
        class header;
        class question;
        class request;
        class response;

        // ---------------------------------------------------------------------
        // codec
        class codec
        {
        public:
            virtual ~codec() = 0;

        public:
            /**
             * Get binary data via move or const ref
             */
            std::vector<std::uint8_t> retrieve();
            const std::vector<std::uint8_t>& binary() const;

            /**
             * Clear binary cache
             */
            virtual void clear();

        public:
            /**
             * Fully qualified domain name detection
             */
            static bool isFqdn(const std::string &name);
            static std::string fqdn(const std::string &name);

        protected:
            std::vector<std::uint8_t> _binary;
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
            static void pack(chen::dns::RRClass valj, std::vector<std::uint8_t> &out);
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
            /**
             * Unpack data
             */
            virtual void unpack(std::int8_t &value);
            virtual void unpack(std::int16_t &value);
            virtual void unpack(std::int32_t &value);
            virtual void unpack(std::int64_t &value);
            virtual void unpack(std::uint8_t &value);
            virtual void unpack(std::uint16_t &value);
            virtual void unpack(std::uint32_t &value);
            virtual void unpack(std::uint64_t &value);
            virtual void unpack(chen::dns::RRType &value);
            virtual void unpack(chen::dns::RRClass &value);
            virtual void unpack(std::string &value, bool domain);
            virtual void unpack(std::vector<std::uint8_t> &value, std::size_t need);

            template <std::size_t Size>
            void unpack(std::array<std::uint8_t, Size> &value)
            {
                auto need = value.size();
                if (this->remain() < need)
                    throw error_size(str::format("dns: codec unpack array size is not enough, require %d bytes", need));

                std::copy(this->_binary.begin() + this->_cursor, this->_binary.begin() + this->_cursor + need, value.begin());
                this->_cursor += need;
            }

        public:
            /**
             * Set binary data
             */
            virtual void assign(const std::vector<std::uint8_t> &value);

            /**
             * Stream state
             */
            virtual std::size_t cursor() const;
            virtual std::size_t remain() const;

        public:
            /**
             * Reset stream pointer
             */
            virtual void reset();

            /**
             * Clear binary cache
             */
            virtual void clear() override;

        protected:
            std::size_t _cursor = 0;
        };
    }
}