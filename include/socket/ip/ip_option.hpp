/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_option.hpp>

namespace chen
{
    // -------------------------------------------------------------------------
    // Base
    class ip_option : public basic_option
    {
    };


    // -------------------------------------------------------------------------
    // IPv4
    class ip_option4 : public ip_option
    {
    public:
        /**
         * IP_TTL(IP packet's ttl value)
         */
        static int ttl(handle_t fd);
        static bool ttl(handle_t fd, int val);
    };


    // -------------------------------------------------------------------------
    // IPv6
    class ip_option6 : public ip_option
    {
    public:
        /**
         * IPV6_V6ONLY(Restricted to sending and receiving IPv6 packets only)
         * normally you can send or recv from both IPv4 and IPv6 address if you use AF_INET6
         */
        static bool v6only(handle_t fd);
        static bool v6only(handle_t fd, bool enable);
    };
}