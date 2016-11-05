/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_endpoint.hpp>
#include <socket/inet/inet_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// address
chen::inet_endpoint::inet_endpoint(std::nullptr_t)
{
}

chen::inet_endpoint::inet_endpoint(const char *mixed)
{
    this->assign(mixed);
}

chen::inet_endpoint::inet_endpoint(const std::string &mixed)
{
    this->assign(mixed);
}

chen::inet_endpoint::inet_endpoint(const ip_address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

chen::inet_endpoint::inet_endpoint(const ip_address &addr, const std::string &service) : _addr(addr), _port(inet_resolver::service(service))
{
}

chen::inet_endpoint::inet_endpoint(const basic_endpoint &ep)
{
    this->assign(ep);
}

chen::inet_endpoint::inet_endpoint(const basic_endpoint &ep, std::uint16_t port)
{
    this->assign(ep, port);
}

chen::inet_endpoint::inet_endpoint(const basic_endpoint &ep, const std::string &service)
{
    this->assign(ep, service);
}

chen::inet_endpoint::inet_endpoint(const struct ::sockaddr *ep)
{
    this->assign(ep);
}

chen::inet_endpoint::inet_endpoint(const struct ::sockaddr *ep, std::uint16_t port)
{
    this->assign(ep, port);
}

chen::inet_endpoint::inet_endpoint(const struct ::sockaddr *ep, const std::string &service)
{
    this->assign(ep, service);
}

// property
std::string chen::inet_endpoint::str(bool cidr, bool scope) const
{
    switch (this->_addr.type())
    {
        case ip_address::Type::IPv4:
            return this->_addr.v4().str(cidr) + ":" + num::str(this->_port);

        case ip_address::Type::IPv6:
            return "[" + this->_addr.v6().str(cidr, scope) + "]:" + num::str(this->_port);

        default:
            return "";
    }
}

bool chen::inet_endpoint::empty() const
{
    return this->_addr.empty();
}

chen::inet_endpoint::operator bool() const
{
    return !this->empty();
}

const chen::ip_address& chen::inet_endpoint::addr() const
{
    return this->_addr;
}

void chen::inet_endpoint::addr(const ip_address &value)
{
    this->_addr = value;
}

std::uint16_t chen::inet_endpoint::port() const
{
    return this->_port;
}

void chen::inet_endpoint::port(std::uint16_t value)
{
    this->_port = value;
}

// special
bool chen::inet_endpoint::isWellKnownPort() const
{
    // from 0 through 1023
    return this->_port <= 1023;
}

bool chen::inet_endpoint::isRegisteredPort() const
{
    // from 1024 through 49151
    return (this->_port >= 1024) && (this->_port <= 49151);
}

bool chen::inet_endpoint::isDynamicPort() const
{
    // from 49152 through 65535
    return this->_port >= 49152;
}

// assignment
void chen::inet_endpoint::assign(std::nullptr_t)
{
    this->_addr = nullptr;
    this->_port = 0;
}

void chen::inet_endpoint::assign(const std::string &mixed)
{
    auto pair = inet_resolver::extract(mixed);
    this->_addr = pair.first;
    this->_port = inet_resolver::service(pair.second);
}

void chen::inet_endpoint::assign(const ip_address &addr, std::uint16_t port)
{
    this->_addr = addr;
    this->_port = port;
}

void chen::inet_endpoint::assign(const ip_address &addr, const std::string &service)
{
    this->_addr = addr;
    this->_port = inet_resolver::service(service);
}

void chen::inet_endpoint::assign(const basic_endpoint &ep)
{
    this->assign((struct ::sockaddr*)&ep.addr);
}

void chen::inet_endpoint::assign(const basic_endpoint &ep, std::uint16_t port)
{
    this->assign((struct ::sockaddr*)&ep.addr, port);
}

void chen::inet_endpoint::assign(const basic_endpoint &ep, const std::string &service)
{
    this->assign((struct ::sockaddr*)&ep.addr, service);
}

void chen::inet_endpoint::assign(const struct ::sockaddr *ep)
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
            this->_addr = ip_version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id);
            this->_port = num::swap(in->sin6_port);
        }
            break;

        default:
            throw std::runtime_error("endpoint: unknown bsd endpoint provided");
    }
}

void chen::inet_endpoint::assign(const struct ::sockaddr *ep, std::uint16_t port)
{
    this->assign(ep);
    this->_port = port;
}

void chen::inet_endpoint::assign(const struct ::sockaddr *ep, const std::string &service)
{
    this->assign(ep);
    this->_port = inet_resolver::service(service);
}

chen::inet_endpoint& chen::inet_endpoint::operator=(std::nullptr_t)
{
    this->assign(nullptr);
    return *this;
}

chen::inet_endpoint& chen::inet_endpoint::operator=(const char *mixed)
{
    this->assign(mixed);
    return *this;
}

chen::inet_endpoint& chen::inet_endpoint::operator=(const std::string &mixed)
{
    this->assign(mixed);
    return *this;
}

chen::inet_endpoint& chen::inet_endpoint::operator=(const basic_endpoint &ep)
{
    this->assign(ep);
    return *this;
}

chen::inet_endpoint& chen::inet_endpoint::operator=(const struct ::sockaddr *ep)
{
    this->assign(ep);
    return *this;
}

// conversion
chen::inet_endpoint::operator chen::basic_endpoint() const
{
    basic_endpoint ret;

    switch (this->_addr.type())
    {
        case ip_address::Type::IPv4:
        {
            auto in = (struct ::sockaddr_in*)&ret.addr;

            in->sin_family      = AF_INET;
            in->sin_port        = chen::num::swap(this->_port);
            in->sin_addr.s_addr = chen::num::swap(this->_addr.v4().addr());

            ret.size = sizeof(*in);
        }
            break;

        case ip_address::Type::IPv6:
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
bool chen::inet_endpoint::operator==(const inet_endpoint &o) const
{
    return (this->_addr == o._addr) && (this->_port == o._port);
}

bool chen::inet_endpoint::operator!=(const inet_endpoint &o) const
{
    return !(*this == o);
}

bool chen::inet_endpoint::operator<(const inet_endpoint &o) const
{
    return (this->_addr == o._addr) ? this->_port < o._port : this->_addr < o._addr;
}

bool chen::inet_endpoint::operator>(const inet_endpoint &o) const
{
    return o < *this;
}

bool chen::inet_endpoint::operator<=(const inet_endpoint &o) const
{
    return (this->_addr == o._addr) ? this->_port <= o._port : this->_addr <= o._addr;
}

bool chen::inet_endpoint::operator>=(const inet_endpoint &o) const
{
    return o <= *this;
}