/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/ip/ip_option.hpp>

// -----------------------------------------------------------------------------
// option
chen::ip::option::option(basic_socket &s) : basic_option(s)
{
}

// ttl
int chen::ip::option::ttl() const
{
    return basic_option::get(IPPROTO_IP, IP_TTL);
}

bool chen::ip::option::ttl(int val)
{
    return basic_option::set(IPPROTO_IP, IP_TTL, val);
}

// v6only
bool chen::ip::option::v6only() const
{
    return basic_option::get(IPPROTO_IPV6, IPV6_V6ONLY) != 0;
}

bool chen::ip::option::v6only(bool enable)
{
    return basic_option::set(IPPROTO_IPV6, IPV6_V6ONLY, enable);
}