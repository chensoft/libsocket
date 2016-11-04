/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/ip/ip_option.hpp>

// -----------------------------------------------------------------------------
// option
chen::ip::option::option(bsd::socket &s) : bsd::option(s)
{
}

// ttl
int chen::ip::option::ttl() const
{
    return bsd::option::get(IPPROTO_IP, IP_TTL);
}

bool chen::ip::option::ttl(int val)
{
    return bsd::option::set(IPPROTO_IP, IP_TTL, val);
}

// v6only
bool chen::ip::option::v6only() const
{
    return bsd::option::get(IPPROTO_IPV6, IPV6_V6ONLY) != 0;
}

bool chen::ip::option::v6only(bool enable)
{
    return bsd::option::set(IPPROTO_IPV6, IPV6_V6ONLY, enable);
}