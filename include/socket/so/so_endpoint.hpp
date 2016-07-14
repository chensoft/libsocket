/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/ip/ip_address.hpp>
#include <memory>

namespace chen
{
    namespace so
    {
        class endpoint
        {
        public:
            /**
             * Construct by ip address and port number
             */
            endpoint(const std::string &addr, std::uint16_t port);  // todo parse both ipv4 and ipv6 address
            endpoint(const chen::ip::address &addr, std::uint16_t port);

        public:
            /**
             * Property
             */
            const chen::ip::address& addr() const;
            std::uint16_t port() const;

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
            std::shared_ptr<chen::ip::address> _addr;
            std::uint16_t _port = 0;
        };
    }
}