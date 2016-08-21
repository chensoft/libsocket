/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_endpoint.hpp>
#include <chen/base/num.hpp>

// -----------------------------------------------------------------------------
// endpoint
chen::net::endpoint::endpoint(std::nullptr_t) : _addr(nullptr)
{
}

chen::net::endpoint::endpoint(const address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

// property
std::string chen::net::endpoint::str(bool cidr, bool scope) const
{
    switch (this->_addr.type())
    {
        case address::Type::IPv4:
            return this->_addr.v4().str(cidr) + ":" + num::str(this->_port);

        case address::Type::IPv6:
            return "[" + this->_addr.v6().str(cidr, scope) + "]:" + num::str(this->_port);

        default:
            return "";
    }
}

bool chen::net::endpoint::empty() const
{
    return this->_addr.empty();
}

const chen::net::address& chen::net::endpoint::addr() const
{
    return this->_addr;
}

void chen::net::endpoint::addr(const address &value)
{
    this->_addr = value;
}

std::uint16_t chen::net::endpoint::port() const
{
    return this->_port;
}

void chen::net::endpoint::port(std::uint16_t value)
{
    this->_port = value;
}

// special
bool chen::net::endpoint::isWellKnownPort() const
{
    // from 0 through 1023
    return this->_port <= 1023;
}

bool chen::net::endpoint::isRegisteredPort() const
{
    // from 1024 through 49151
    return (this->_port >= 1024) && (this->_port <= 49151);
}

bool chen::net::endpoint::isDynamicPort() const
{
    // from 49152 through 65535
    return this->_port >= 49152;
}

// operator
bool chen::net::endpoint::operator==(const endpoint &o) const
{
    return (this->_addr == o._addr) && (this->_port == o._port);
}

bool chen::net::endpoint::operator!=(const endpoint &o) const
{
    return !(*this == o);
}

bool chen::net::endpoint::operator<(const endpoint &o) const
{
    return (this->_addr == o._addr) ? this->_port < o._port : this->_addr < o._addr;
}

bool chen::net::endpoint::operator>(const endpoint &o) const
{
    return o < *this;
}

bool chen::net::endpoint::operator<=(const endpoint &o) const
{
    return (this->_addr == o._addr) ? this->_port <= o._port : this->_addr <= o._addr;
}

bool chen::net::endpoint::operator>=(const endpoint &o) const
{
    return o <= *this;
}

chen::net::endpoint::operator bool() const
{
    return !this->empty();
}