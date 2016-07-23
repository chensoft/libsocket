/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "dns_define.hpp"
#include "dns_error.hpp"
#include <chen/base/iterator.hpp>
#include <chen/base/str.hpp>
#include <vector>
#include <array>
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
            typedef chen::forward_iterator<const std::uint8_t, const std::uint8_t> iterator;

        public:
            virtual ~codec() = 0;

        public:
            /**
             * Fully qualified domain name detection
             */
            static bool isFqdn(const std::string &name);
            static std::string& fqdn(std::string &name);
            static std::string fqdn(const std::string &name);

            /**
             * Partially qualified domain name detection
             */
            static bool isPqdn(const std::string &name, const std::string &fqdn);
        };


        // ---------------------------------------------------------------------
        // encoder
        class encoder : public codec
        {
        public:
            /**
             * Property
             */
            const std::vector<std::uint8_t>& data() const;
            std::size_t size() const;

            /**
             * Reset state
             */
            void reset();

        public:
            /**
             * Change specific byte
             */
            void change(std::size_t pos, std::uint8_t byte);

        public:
            /**
             * Pack data
             */
            void pack(std::int8_t val);
            void pack(std::int16_t val);
            void pack(std::int32_t val);
            void pack(std::int64_t val);
            void pack(std::uint8_t val);
            void pack(std::uint16_t val);
            void pack(std::uint32_t val);
            void pack(std::uint64_t val);
            void pack(chen::dns::RRType val);
            void pack(chen::dns::RRClass val);
            void pack(const std::string &val, bool domain);
            void pack(const std::vector<std::uint8_t> &val, std::size_t need);

            template <std::size_t Size>
            void pack(const std::array<std::uint8_t, Size> &val)
            {
                this->_data.insert(this->_data.end(), val.begin(), val.end());
            }

        protected:
            std::vector<std::uint8_t> _data;
        };


        // ---------------------------------------------------------------------
        // decoder
        class decoder : public codec
        {
        public:
            /**
             * Ensure the iterator is valid during decoding
             */
            decoder(iterator beg, iterator end);

        public:
            /**
             * Property
             */
            const decoder::iterator& beg() const;
            const decoder::iterator& cur() const;
            const decoder::iterator& end() const;

            const std::map<std::size_t, std::string>& cache() const;

            /**
             * Reset state
             */
            void reset();

        public:
            /**
             * Unpack data
             */
            void unpack(std::int8_t &val);
            void unpack(std::int16_t &val);
            void unpack(std::int32_t &val);
            void unpack(std::int64_t &val);
            void unpack(std::uint8_t &val);
            void unpack(std::uint16_t &val);
            void unpack(std::uint32_t &val);
            void unpack(std::uint64_t &val);
            void unpack(chen::dns::RRType &val);
            void unpack(chen::dns::RRClass &val);
            void unpack(std::string &val, bool domain);
            void unpack(std::vector<std::uint8_t> &val, std::size_t need);

            template <std::size_t Size>
            void unpack(std::array<std::uint8_t, Size> &val)
            {
                auto need = val.size();
                if (std::distance(this->_cur, this->_end) < need)
                    throw error_size(str::format("dns: codec unpack array size is not enough, require %d bytes", need));

                auto from = this->_cur;

                std::advance(this->_cur, need);
                std::copy(from, this->_cur, val.begin());
            }

        protected:
            iterator _beg;
            iterator _cur;
            iterator _end;

            std::map<std::size_t, std::string> _cache;  // compression cache
        };
    }
}