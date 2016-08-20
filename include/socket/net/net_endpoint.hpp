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
            endpoint(std::nullptr_t);
            endpoint(const address &addr, std::uint16_t port);

        public:
            /**
             * Property
             */
            bool empty() const;

            const address& addr() const;
            void addr(const address &value);

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

            operator bool() const;

        private:
            address _addr;
            std::uint16_t _port = 0;
        };
    }
}