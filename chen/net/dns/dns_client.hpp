/**
 * Created by Jian Chen
 * @since  2015.12.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_packet.hpp"
#include <chen/net/udp/udp_client.hpp>

namespace chen
{
    namespace dns
    {
        class client
        {
        public:
            virtual ~client() = default;

        public:
            /**
             * Set nameserver address
             */
            void setNameserver(const std::string &addr, std::uint16_t port);

            /**
             * Resolve domain name
             */
            chen::dns::response query(const std::string &qname,
                                      chen::dns::RRType qtype,
                                      chen::dns::RRClass qclass = chen::dns::RRClass::IN);

        protected:
            std::string _ns_addr;
            std::uint16_t _ns_port = 0;

            chen::udp::client _udp;
        };
    }
}