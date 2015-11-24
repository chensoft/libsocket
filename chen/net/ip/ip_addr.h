/**
 * Created by Jian Chen
 * @since  2015.08.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>

namespace chen
{
    namespace ip
    {
        class address
        {
        public:
            address() = default;
            virtual ~address() = default;

            explicit address(std::uint32_t addr);
            explicit address(std::uint32_t addr, std::uint8_t subnet);

            explicit address(const std::string &addr);
            explicit address(const std::string &addr, std::uint8_t subnet);

        public:
            /**
             * Whether the address is empty
             */
            virtual bool empty() const;

            /**
             * Whether the address is loopback
             */
            virtual bool is_loopback() const;

            /**
             * Whether the address is broadcast
             */
            virtual bool is_broadcast() const;

            /**
             * Whether the address is multicast
             */
            virtual bool is_multicast() const;

        public:
            /**
             * IP address to string
             */
            virtual std::string str() const;

        public:
            /**
             * Whether the network is class A
             */
            virtual bool is_class_a() const;

            /**
             * Whether the network is class B
             */
            virtual bool is_class_b() const;

            /**
             * Whether the network is class C
             */
            virtual bool is_class_c() const;

        public:
            /**
             * Decimal address
             */
            std::uint32_t addr() const;

            /**
             * Subnet mask
             */
            std::uint32_t mask() const;

            /**
             * Full address/subnet
             */
            std::string full() const;

            /**
             * Subnet length
             */
            std::uint8_t subnet() const;

            /**
             * Subnet wildcard
             */
            std::uint32_t wildcard() const;

        public:
            /**
             * Network address
             */
            address network() const;

            /**
             * Min host address under this network
             */
            address host_min() const;

            /**
             * Max host address under this network
             */
            address host_max() const;

            /**
             * Broadcast address based on this ip
             */
            address broadcast() const;

        public:
            /**
             * Assign integer to address
             */
            void assign(std::uint32_t addr, std::uint8_t subnet = 0);

            /**
             * Assign string to address
             */
            void assign(const std::string &addr, std::uint8_t subnet = 0);

        public:
            /**
             * Convert int ip to string
             */
            static std::string to_string(std::uint32_t addr);

            /**
             * Convert string ip to int
             */
            static std::uint32_t to_integer(const std::string &addr);

            /**
             * A loopback address
             */
            static address loopback();

        protected:
            std::uint32_t _addr = 0;
            std::uint32_t _mask = 0;
        };

        /**
         * Operator
         */
        bool operator==(const address &a, const address &b);
        bool operator!=(const address &a, const address &b);

        bool operator<(const address &a, const address &b);
        bool operator>(const address &a, const address &b);

        bool operator<=(const address &a, const address &b);
        bool operator>=(const address &a, const address &b);
    }
}