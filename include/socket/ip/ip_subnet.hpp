/**
 * Created by Jian Chen
 * @since  2016.08.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "ip_address.hpp"

namespace chen
{
    namespace ip
    {
        // ---------------------------------------------------------------------
        // IPv4 subnet
        class subnet_v4 : public address_v4
        {
        public:
            subnet_v4() = default;

            /**
             * Construct by IPv4 dotted decimal string and CIDR prefix
             * :-) subnet_v4("127.0.0.1")
             * :-) subnet_v4("127.0.0.1/8")
             * :-) subnet_v4("127.0.0.1", 8)
             * :-) subnet_v4("127.0.0.1", "255.0.0.0")
             * Accept some special address format
             * :-) subnet_v4("127")       -> "127.0.0.0"
             * :-) subnet_v4("127.1")     -> "127.0.0.1"
             * :-) subnet_v4("192.168.1") -> "192.168.0.1"
             * @caution default CIDR prefix will be 32 if you don't provide
             */
            subnet_v4(const std::string &addr);
            subnet_v4(const std::string &addr, std::uint8_t cidr);
            subnet_v4(const std::string &addr, const std::string &mask);

            /**
             * Construct by IPv4 integer and CIDR prefix
             * e.g: "127.0.0.1" is 0x7F000001, also is 2130706433
             * :-) subnet_v4(2130706433)
             * :-) subnet_v4(0x7F000001)
             * :-) subnet_v4(0x7F000001, 8)
             * :-) subnet_v4(0x7F000001, "255.0.0.0")
             * @caution default CIDR prefix will be 32 if you don't provide
             */
            subnet_v4(std::uint32_t addr);
            subnet_v4(std::uint32_t addr, std::uint8_t cidr);
            subnet_v4(std::uint32_t addr, const std::string &mask);

            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const override;

        public:
            /**
             * Assignment
             */
            virtual void assign(const std::string &addr) override;
            virtual void assign(const std::string &addr, std::uint8_t cidr);
            virtual void assign(const std::string &addr, const std::string &mask);

            virtual void assign(std::uint32_t addr) override;
            virtual void assign(std::uint32_t addr, std::uint8_t cidr);
            virtual void assign(std::uint32_t addr, const std::string &mask);

            virtual address& operator=(const std::string &addr) override;
            virtual address& operator=(std::uint32_t addr) override;

        public:
            /**
             * Standard canonical representation(with CIDR notation)
             * e.g: 127.0.0.1/8
             */
            virtual std::string str() const override;

            /**
             * Get raw value
             */
            const std::uint8_t& cidr() const;
            std::uint8_t& cidr();

            /**
             * Netmask & Wildcard mask
             */
            std::uint32_t netmask() const;
            std::uint32_t wildcard() const;

        public:
            /**
             * Network address based on current ip
             */
            subnet_v4 network() const;
            subnet_v4 minhost() const;
            subnet_v4 maxhost() const;
            subnet_v4 broadcast() const;

            /**
             * Calculate hosts count based on this network
             */
            std::size_t hosts() const;

        public:
            /**
             * Special addressing type
             */
            bool isBroadcast() const;

        public:
            /**
             * Operator, compare based on IPv4 address
             * @caution consider CIDR prefix when address is equal
             */
            virtual bool operator==(const address &o) const override;
            virtual bool operator<(const address &o) const override;
            virtual bool operator<=(const address &o) const override;

        protected:
            std::uint8_t _cidr = 32;  // CIDR notation prefix length
        };


        // ---------------------------------------------------------------------
        // IPv6 subnet
        class subnet_v6 : public address_v6
        {
        public:
            /**
             * Clone current object
             */
            virtual std::shared_ptr<chen::ip::address> clone() const override;
        };
    }
}