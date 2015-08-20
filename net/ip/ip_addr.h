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
        // ---------------------------------------------------------------------
        // address base class
        class address
        {
        public:
            address() = default;
            virtual ~address() = default;

        public:
            /**
             * Whether the address is empty
             */
            virtual bool empty() const = 0;

            /**
             * Whether the address is loopback
             */
            virtual bool is_loopback() const = 0;

            /**
             * Whether the address is broadcast
             */
            virtual bool is_broadcast() const = 0;

            /**
             * Whether the address is multicast
             */
            virtual bool is_multicast() const = 0;

        public:
            /**
             * IP address to string
             */
            virtual std::string str() const = 0;
        };


        // ---------------------------------------------------------------------
        // address v4
        class address_v4 : public address
        {
        public:
            address_v4() = default;
            address_v4(std::uint32_t addr, std::uint8_t subnet = 0);
            address_v4(const std::string &addr, std::uint8_t subnet = 0);

        public:
            /**
             * Whether the address is empty
             */
            virtual bool empty() const override;

            /**
             * Whether the address is loopback
             */
            virtual bool is_loopback() const override;

            /**
             * Whether the address is broadcast
             */
            virtual bool is_broadcast() const override;

            /**
             * Whether the address is multicast
             */
            virtual bool is_multicast() const override;

        public:
            /**
             * IP address to string
             */
            virtual std::string str() const override;

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
            address_v4 network() const;

            /**
             * Min host address under this network
             */
            address_v4 host_min() const;

            /**
             * Max host address under this network
             */
            address_v4 host_max() const;

            /**
             * Broadcast address based on this ip
             */
            address_v4 broadcast() const;

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
            static address_v4 loopback();

        protected:
            std::uint32_t _addr = 0;
            std::uint32_t _mask = 0;
        };


        // ---------------------------------------------------------------------
        // address v6
        class address_v6 : public address
        {
        public:
        };
    }
}