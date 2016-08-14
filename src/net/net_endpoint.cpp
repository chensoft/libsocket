/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_endpoint.hpp>

using namespace chen;
using namespace chen::net;

// -----------------------------------------------------------------------------
// endpoint
endpoint::endpoint(const std::string &addr, std::uint16_t port) : endpoint(net::address(addr), port)
{
}

endpoint::endpoint(const net::address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

// property
const net::address& endpoint::addr() const
{
    return this->_addr;
}

void endpoint::addr(const net::address &value)
{
    this->_addr = value;
}

std::uint16_t endpoint::port() const
{
    return this->_port;
}

void endpoint::port(std::uint16_t value)
{
    this->_port = value;
}

// operator
bool endpoint::operator==(const endpoint &o) const
{
    return (this->_addr == o._addr) && (this->_port == o._port);
}

bool endpoint::operator!=(const endpoint &o) const
{
    return !(*this == o);
}

bool endpoint::operator<(const endpoint &o) const
{
    return (this->_addr == o._addr) ? this->_port < o._port : this->_addr < o._addr;
}

bool endpoint::operator>(const endpoint &o) const
{
    return o < *this;
}

bool endpoint::operator<=(const endpoint &o) const
{
    return (this->_addr == o._addr) ? this->_port <= o._port : this->_addr <= o._addr;
}

bool endpoint::operator>=(const endpoint &o) const
{
    return o <= *this;
}