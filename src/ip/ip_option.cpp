/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/ip/ip_option.hpp"

// -----------------------------------------------------------------------------
// ip_option4

// ttl
int chen::ip_option4::ttl(handle_t fd)
{
    return basic_option::get(fd, IPPROTO_IP, IP_TTL);
}

bool chen::ip_option4::ttl(handle_t fd, int val)
{
    return basic_option::set(fd, IPPROTO_IP, IP_TTL, val);
}


// -----------------------------------------------------------------------------
// ip_option6

// v6only
bool chen::ip_option6::v6only(handle_t fd)
{
    return basic_option::get(fd, IPPROTO_IPV6, IPV6_V6ONLY) != 0;
}

bool chen::ip_option6::v6only(handle_t fd, bool enable)
{
    return basic_option::set(fd, IPPROTO_IPV6, IPV6_V6ONLY, enable);
}