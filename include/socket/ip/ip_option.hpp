/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/bsd_option.hpp>

namespace chen
{
    class ip_option : public bsd_option
    {
    public:
        ip_option(bsd_socket &s);

    public:
        /**
         * IP_TTL(IP packet's ttl value)
         */
        int ttl() const;
        bool ttl(int val);

        /**
         * IPV6_V6ONLY(Restricted to sending and receiving IPv6 packets only)
         * normally you can send or recv from both IPv4 and IPv6 address if you use AF_INET6
         */
        bool v6only() const;
        bool v6only(bool enable);
    };
}