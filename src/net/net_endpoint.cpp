/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_endpoint.hpp>
#include <socket/net/net_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// address
chen::net::endpoint::endpoint(std::nullptr_t)
{
    *this = nullptr;
}

chen::net::endpoint::endpoint(const char *mixed)
{
    *this = mixed;
}

chen::net::endpoint::endpoint(const std::string &mixed)
{
    *this = mixed;
}

chen::net::endpoint::endpoint(const std::string &mixed, ip::address::Type type)
{
    resolver::first(mixed, type, *this);
}

chen::net::endpoint::endpoint(const std::string &host, const std::string &service)
{
    resolver::first(host, service, *this);
}

chen::net::endpoint::endpoint(const std::string &host, const std::string &service, ip::address::Type type)
{
    resolver::first(host, service, type, *this);
}

chen::net::endpoint::endpoint(const std::string &host, std::uint16_t port)
{
    resolver::first(host, port, *this);
}

chen::net::endpoint::endpoint(const std::string &host, std::uint16_t port, ip::address::Type type)
{
    resolver::first(host, port, type, *this);
}

chen::net::endpoint::endpoint(const bsd::endpoint &ep)
{
    *this = ep;
}

chen::net::endpoint::endpoint(const struct ::sockaddr *ep)
{
    *this = ep;
}

// property
std::string chen::net::endpoint::str(bool cidr, bool scope) const
{
    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
            return this->_addr.v4().str(cidr) + ":" + num::str(this->_port);

        case ip::address::Type::IPv6:
            return "[" + this->_addr.v6().str(cidr, scope) + "]:" + num::str(this->_port);

        default:
            return "";
    }
}

bool chen::net::endpoint::empty() const
{
    return this->_addr.empty();
}

chen::net::endpoint::operator bool() const
{
    return !this->empty();
}

const chen::ip::address& chen::net::endpoint::addr() const
{
    return this->_addr;
}

void chen::net::endpoint::addr(const ip::address &value)
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

// assignment
chen::net::endpoint& chen::net::endpoint::operator=(std::nullptr_t)
{
    this->_addr = nullptr;
    this->_port = 0;
    return *this;
}

chen::net::endpoint& chen::net::endpoint::operator=(const char *mixed)
{
    return *this = std::string(mixed);
}

chen::net::endpoint& chen::net::endpoint::operator=(const std::string &mixed)
{
    resolver::first(mixed, *this);
    return *this;
}

chen::net::endpoint& chen::net::endpoint::operator=(const bsd::endpoint &ep)
{
    return *this = (struct ::sockaddr*)&ep.addr;
}

chen::net::endpoint& chen::net::endpoint::operator=(const struct ::sockaddr *ep)
{
    switch (ep->sa_family)
    {
        case AF_INET:
        {
            auto in = (struct ::sockaddr_in*)ep;
            this->_addr = num::swap(in->sin_addr.s_addr);
            this->_port = num::swap(in->sin_port);
        }
            break;

        case AF_INET6:
        {
            auto in = (struct ::sockaddr_in6*)ep;
            this->_addr = ip::version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id);
            this->_port = num::swap(in->sin6_port);
        }
            break;

        default:
            throw std::runtime_error("endpoint: unknown bsd endpoint provided");
    }

    return *this;
}

// conversion
chen::net::endpoint::operator bsd::endpoint() const
{
    bsd::endpoint ret;

    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
        {
            auto in = (struct ::sockaddr_in*)&ret.addr;

            in->sin_family      = AF_INET;
            in->sin_port        = chen::num::swap(this->_port);
            in->sin_addr.s_addr = chen::num::swap(this->_addr.v4().addr());

            ret.size = sizeof(*in);
        }
            break;

        case ip::address::Type::IPv6:
        {
            auto in = (struct ::sockaddr_in6*)&ret.addr;

            in->sin6_family   = AF_INET6;
            in->sin6_port     = chen::num::swap(this->_port);
            in->sin6_scope_id = this->_addr.v6().scope();

            ::memcpy(in->sin6_addr.s6_addr, this->_addr.v6().addr().data(), 16);

            ret.size = sizeof(*in);
        }
            break;

        default:
            break;
    }

    return ret;
}

// comparison
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