/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "dns_packet.h"

namespace chen
{
    namespace dns
    {
        class client
        {
        public:
            client();

        public:
            /**
             * Resolve domain name
             */
            chen::dns::response resolve(const std::string &qname,
                                        chen::dns::RRType qtype,
                                        chen::dns::RRClass qclass = chen::dns::RRClass::IN);
        };
    }
}