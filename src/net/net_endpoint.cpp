/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_endpoint.hpp>

// -----------------------------------------------------------------------------
// endpoint
chen::net::endpoint::endpoint(std::nullptr_t) : _addr(nullptr)
{
}

chen::net::endpoint::endpoint(const std::string &addr, std::uint16_t port) : endpoint(address(addr), port)
{
}

chen::net::endpoint::endpoint(const address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

// property
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