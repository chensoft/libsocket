/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_option.hpp>

// -----------------------------------------------------------------------------
// option

// ttl
int chen::ip::option::ttl() const
{
    return bsd::option::intVal(IPPROTO_IP, IP_TTL);
}

bool chen::ip::option::ttl(int val)
{
    return !::setsockopt(this->_fd, IPPROTO_IP, IP_TTL, &val, sizeof(val));
}