/**
 * Created by Jian Chen
 * @since  2016.09.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/bsd_address.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// address
chen::bsd::address::address() : ::sockaddr_storage()  // init storage to zeros
{
}

bool chen::bsd::address::operator==(const address &o) const
{
    return ::memcmp(this, &o, sizeof(::sockaddr_storage)) == 0;
}

bool chen::bsd::address::operator!=(const address &o) const
{
    return !(*this == o);
}

bool chen::bsd::address::operator<(const address &o) const
{
    return ::memcmp(this, &o, sizeof(::sockaddr_storage)) < 0;
}

bool chen::bsd::address::operator>(const address &o) const
{
    return o < *this;
}

bool chen::bsd::address::operator<=(const address &o) const
{
    return ::memcmp(this, &o, sizeof(::sockaddr_storage)) > 0;
}

bool chen::bsd::address::operator>=(const address &o) const
{
    return o <= *this;
}