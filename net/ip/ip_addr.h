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
            virtual bool loopback() const = 0;

            /**
             * Whether the address is broadcast
             */
            virtual bool broadcast() const = 0;

            /**
             * Whether the address is multicast
             */
            virtual bool multicast() const = 0;

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
            address_v4(std::uint32_t addr, std::uint32_t mask = 0);
            address_v4(const std::string &addr, std::uint32_t mask = 0);

        public:
            /**
             * Whether the address is empty
             */
            virtual bool empty() const override;

            /**
             * Whether the address is loopback
             */
            virtual bool loopback() const override;

            /**
             * Whether the address is broadcast
             */
            virtual bool broadcast() const override;

            /**
             * Whether the address is multicast
             */
            virtual bool multicast() const override;

        public:
            /**
             * IP address to string
             */
            virtual std::string str() const override;

        public:
            /**
             * Whether the network is class A
             */
            virtual bool class_a() const;

            /**
             * Whether the network is class B
             */
            virtual bool class_b() const;

            /**
             * Whether the network is class C
             */
            virtual bool class_c() const;

        public:
            /**
             * Decimal address
             */
            std::uint32_t addr() const;

            /**
             * Subnet mask
             */
            std::uint32_t mask() const;

        public:
            /**
             * Assign integer to address
             */
            void assign(std::uint32_t addr, std::uint32_t mask = 0);

            /**
             * Assign string to address
             */
            void assign(const std::string &addr, std::uint32_t mask = 0);

        public:
            /**
             * Convert int ip to string
             */
            static std::string to_string(std::uint32_t addr);

            /**
             * Convert string ip to int
             */
            static std::uint32_t to_integer(const std::string &addr);

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