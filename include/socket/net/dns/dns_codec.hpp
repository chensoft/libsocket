/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/dns/dns_define.hpp>
#include <chen/base/iterator.hpp>
#include <chen/base/str.hpp>
#include <unordered_map>
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
            enum class StringType {Plain = 1, Domain};

            typedef forward_iterator<const std::uint8_t> iterator;

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
            std::vector<std::uint8_t> move();
            std::size_t size() const;

            const std::unordered_map<std::string, std::uint16_t>& cache() const;

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
            template <typename IntType>
            void pack(IntType val)
            {
                for (int i = sizeof(val); i >= 0; i -= 8)
                    this->_data.emplace_back(static_cast<std::uint8_t>(val >> i & 0xFF));
            }

            void pack(RRType val);
            void pack(RRClass val);
            void pack(edns0::OptCode val);
            void pack(const std::string &val, StringType type, bool compress);
            void pack(const std::vector<std::uint8_t> &val, std::size_t need);

            template <std::size_t Size>
            void pack(const std::array<std::uint8_t, Size> &val)
            {
                this->_data.insert(this->_data.end(), val.begin(), val.end());
            }

        protected:
            void plain(const std::string &val);
            void domain(const std::string &val, bool compress);
            bool compress(const std::string &val);

        protected:
            std::vector<std::uint8_t> _data;
            std::unordered_map<std::string, std::uint16_t> _cache;  // compression cache
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

            /**
             * Reset state
             */
            void reset();

        public:
            /**
             * Unpack data
             */
            template <typename IntType>
            void unpack(IntType &val)
            {
                int len = sizeof(val);

                if (std::distance(this->_cur, this->_end) < len)
                    throw std::runtime_error(chen::str::format("dns: codec unpack size is not enough, require %d bytes", len));

                val = 0;

                for (int i = 0; i < len; ++i)
                    val |= static_cast<IntType>(*this->_cur++) << (len - i - 1) * 8;
            }

            void unpack(RRType &val);
            void unpack(RRClass &val);
            void unpack(edns0::OptCode &val);
            void unpack(std::string &val, StringType type);
            void unpack(std::vector<std::uint8_t> &val, std::size_t need);

            template <std::size_t Size>
            void unpack(std::array<std::uint8_t, Size> &val)
            {
                auto need = val.size();
                if (static_cast<std::size_t>(std::distance(this->_cur, this->_end)) < need)
                    throw std::runtime_error(str::format("dns: codec unpack array size is not enough, require %d bytes", need));

                auto from = this->_cur;

                std::advance(this->_cur, need);
                std::copy(from, this->_cur, val.begin());
            }

        protected:
            void plain(std::string &val);
            void domain(std::string &val);
            void extract(std::string &val, iterator &cur);

        protected:
            iterator _beg;
            iterator _cur;
            iterator _end;
        };
    }
}