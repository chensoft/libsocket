/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/so/so_endpoint.hpp>

using namespace chen;
using namespace chen::so;

// -----------------------------------------------------------------------------
// endpoint
endpoint::endpoint(const std::string &addr, std::uint16_t port)
: _addr(new chen::ip::address_v4(addr))
, _port(port)
{
}

endpoint::endpoint(const chen::ip::address &addr, std::uint16_t port)
: _addr(addr.clone())
, _port(port)
{
}

// property
const chen::ip::address& endpoint::addr() const
{
    return *this->_addr;
}

std::uint16_t endpoint::port() const
{
    return this->_port;
}

// operator
bool endpoint::operator==(const endpoint &o) const
{
    return (*this->_addr == *o._addr) && (this->_port == o._port);
}

bool endpoint::operator!=(const endpoint &o) const
{
    return !(*this == o);
}

bool endpoint::operator<(const endpoint &o) const
{
    return (*this->_addr == *o._addr) ? this->_port < o._port : *this->_addr < *o._addr;
}

bool endpoint::operator>(const endpoint &o) const
{
    return o < *this;
}

bool endpoint::operator<=(const endpoint &o) const
{
    return (*this->_addr == *o._addr) ? this->_port <= o._port : *this->_addr < *o._addr;
}

bool endpoint::operator>=(const endpoint &o) const
{
    return o <= *this;
}