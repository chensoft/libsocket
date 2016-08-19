/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_address.hpp>

namespace chen
{
    namespace net
    {
        class endpoint
        {
        public:
            explicit endpoint(std::nullptr_t);
            endpoint(const std::string &addr, std::uint16_t port);
            endpoint(const net::address &addr, std::uint16_t port);

        public:
            /**
             * Property
             */
            bool empty() const;

            const net::address& addr() const;
            void addr(const net::address &value);

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
            net::address _addr;
            std::uint16_t _port = 0;
        };
    }
}