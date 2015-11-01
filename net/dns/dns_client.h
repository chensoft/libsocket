/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_packet.h"
#include "../ip/ip_addr.h"
#include "../udp/udp_client.h"

namespace chen
{
    namespace dns
    {
        class client
        {
        public:
            client();
            virtual ~client() = default;

        public:
            /**
             * Set resolver address
             */
            void setNameserver(const std::vector<chen::ip::address_v4> &list);

            /**
             * Resolve domain name
             */
            chen::dns::response resolve(const std::string &qname,
                                        chen::dns::RRType qtype,
                                        chen::dns::RRClass qclass = chen::dns::RRClass::IN);

        protected:
            std::vector<chen::ip::address_v4> _server;

            chen::udp::client _udp;
        };
    }
}