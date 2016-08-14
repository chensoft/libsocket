/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_address.hpp>

namespace chen
{
    namespace so
    {
        class endpoint
        {
        public:
            endpoint(const std::string &addr, std::uint16_t port);
            endpoint(const ip::address &addr, std::uint16_t port);

        public:
            /**
             * Property
             */
            const ip::address& addr() const;
            void addr(const ip::address &value);

            std::uint16_t port() const;
            void port(std::uint16_t value);

        public:
            /**
             * Operator
             */
            bool operator==(const endpoint &o) const;
            bool operator!=(const endpoint &o) const;

            bool operator<(const endpoint &o) const;
            bool operator>(const endpoint &o) const;

            bool operator<=(const endpoint &o) const;
            bool operator>=(const endpoint &o) const;

        private:
            ip::address _addr;
            std::uint16_t _port = 0;
        };
    }
}