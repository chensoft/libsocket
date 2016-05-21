/**
 * Created by Jian Chen
 * @since  2015.08.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
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

            explicit address(std::uint32_t addr, std::uint8_t subnet = 0);
            explicit address(const std::string &addr, std::uint8_t subnet = 0);

        public:
            /**
             * Whether the address is empty
             */
            virtual bool empty() const;

            /**
             * Whether the address is loopback
             */
            virtual bool isLoopback() const;

            /**
             * Whether the address is broadcast
             */
            virtual bool isBroadcast() const;

            /**
             * Whether the address is multicast
             */
            virtual bool isMulticast() const;

        public:
            /**
             * Whether the network is class A
             */
            virtual bool isClassA() const;

            /**
             * Whether the network is class B
             */
            virtual bool isClassB() const;

            /**
             * Whether the network is class C
             */
            virtual bool isClassC() const;

        public:
            /**
             * IP address to string
             */
            virtual std::string str() const;

            /**
             * Decimal address
             */
            virtual std::uint32_t addr() const;

            /**
             * Subnet mask
             */
            virtual std::uint32_t mask() const;

            /**
             * Full address/subnet
             */
            virtual std::string full() const;

            /**
             * Subnet length
             */
            virtual std::uint8_t subnet() const;

            /**
             * Subnet wildcard
             */
            virtual std::uint32_t wildcard() const;

        public:
            /**
             * Network address
             */
            virtual address network() const;

            /**
             * Min host address under this network
             */
            virtual address hostMin() const;

            /**
             * Max host address under this network
             */
            virtual address hostMax() const;

            /**
             * Broadcast address based on this ip
             */
            virtual address broadcast() const;

        public:
            /**
             * Assign integer to address
             */
            virtual void assign(std::uint32_t addr, std::uint8_t subnet = 0);

            /**
             * Assign string to address
             */
            virtual void assign(const std::string &addr, std::uint8_t subnet = 0);

        public:
            /**
             * Convert int ip to string
             */
            static std::string toString(std::uint32_t addr);

            /**
             * Convert string ip to int
             */
            static std::uint32_t toInteger(const std::string &addr);

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