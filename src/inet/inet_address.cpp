/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_address.hpp>
#include <socket/inet/inet_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// address
chen::inet_address::inet_address(std::nullptr_t)
{
}

chen::inet_address::inet_address(const char *mixed)
{
    this->assign(mixed);
}

chen::inet_address::inet_address(const std::string &mixed)
{
    this->assign(mixed);
}

chen::inet_address::inet_address(const ip_address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

chen::inet_address::inet_address(const ip_address &addr, const std::string &service) : _addr(addr), _port(inet_resolver::service(service))
{
}

chen::inet_address::inet_address(const basic_address &addr)
{
    this->assign(addr);
}

chen::inet_address::inet_address(const basic_address &addr, std::uint16_t port)
{
    this->assign(addr, port);
}

chen::inet_address::inet_address(const basic_address &addr, const std::string &service)
{
    this->assign(addr, service);
}

chen::inet_address::inet_address(const struct ::sockaddr *addr)
{
    this->assign(addr);
}

chen::inet_address::inet_address(const struct ::sockaddr *addr, std::uint16_t port)
{
    this->assign(addr, port);
}

chen::inet_address::inet_address(const struct ::sockaddr *addr, const std::string &service)
{
    this->assign(addr, service);
}

// property
std::string chen::inet_address::str(bool cidr, bool scope) const
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

bool chen::inet_address::empty() const
{
    return this->_addr.empty();
}

chen::inet_address::operator bool() const
{
    return !this->empty();
}

const chen::ip_address& chen::inet_address::addr() const
{
    return this->_addr;
}

void chen::inet_address::addr(const ip_address &value)
{
    this->_addr = value;
}

std::uint16_t chen::inet_address::port() const
{
    return this->_port;
}

void chen::inet_address::port(std::uint16_t value)
{
    this->_port = value;
}

// special
bool chen::inet_address::isWellKnownPort() const
{
    // from 0 through 1023
    return this->_port <= 1023;
}

bool chen::inet_address::isRegisteredPort() const
{
    // from 1024 through 49151
    return (this->_port >= 1024) && (this->_port <= 49151);
}

bool chen::inet_address::isDynamicPort() const
{
    // from 49152 through 65535
    return this->_port >= 49152;
}

// assignment
void chen::inet_address::assign(std::nullptr_t)
{
    this->_addr = nullptr;
    this->_port = 0;
}

void chen::inet_address::assign(const std::string &mixed)
{
    auto pair = inet_resolver::extract(mixed);
    this->_addr = pair.first;
    this->_port = inet_resolver::service(pair.second);
}

void chen::inet_address::assign(const ip_address &addr, std::uint16_t port)
{
    this->_addr = addr;
    this->_port = port;
}

void chen::inet_address::assign(const ip_address &addr, const std::string &service)
{
    this->_addr = addr;
    this->_port = inet_resolver::service(service);
}

void chen::inet_address::assign(const basic_address &addr)
{
    this->assign((struct ::sockaddr*)&addr.addr);
}

void chen::inet_address::assign(const basic_address &addr, std::uint16_t port)
{
    this->assign((struct ::sockaddr*)&addr.addr, port);
}

void chen::inet_address::assign(const basic_address &addr, const std::string &service)
{
    this->assign((struct ::sockaddr*)&addr.addr, service);
}

void chen::inet_address::assign(const struct ::sockaddr *addr)
{
    switch (addr->sa_family)
    {
        case AF_INET:
        {
            auto in = (struct ::sockaddr_in*)addr;
            this->_addr = num::swap(in->sin_addr.s_addr);
            this->_port = num::swap(in->sin_port);
        }
            break;

        case AF_INET6:
        {
            auto in = (struct ::sockaddr_in6*)addr;
            this->_addr = ip_version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id);
            this->_port = num::swap(in->sin6_port);
        }
            break;

        default:
            throw std::runtime_error("address: unknown bsd address provided");
    }
}

void chen::inet_address::assign(const struct ::sockaddr *addr, std::uint16_t port)
{
    this->assign(addr);
    this->_port = port;
}

void chen::inet_address::assign(const struct ::sockaddr *addr, const std::string &service)
{
    this->assign(addr);
    this->_port = inet_resolver::service(service);
}

chen::inet_address& chen::inet_address::operator=(std::nullptr_t)
{
    this->assign(nullptr);
    return *this;
}

chen::inet_address& chen::inet_address::operator=(const char *mixed)
{
    this->assign(mixed);
    return *this;
}

chen::inet_address& chen::inet_address::operator=(const std::string &mixed)
{
    this->assign(mixed);
    return *this;
}

chen::inet_address& chen::inet_address::operator=(const basic_address &addr)
{
    this->assign(addr);
    return *this;
}

chen::inet_address& chen::inet_address::operator=(const struct ::sockaddr *addr)
{
    this->assign(addr);
    return *this;
}

// conversion
chen::inet_address::operator chen::basic_address() const
{
    basic_address ret;

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
bool chen::inet_address::operator==(const inet_address &o) const
{
    return (this->_addr == o._addr) && (this->_port == o._port);
}

bool chen::inet_address::operator!=(const inet_address &o) const
{
    return !(*this == o);
}

bool chen::inet_address::operator<(const inet_address &o) const
{
    return (this->_addr == o._addr) ? this->_port < o._port : this->_addr < o._addr;
}

bool chen::inet_address::operator>(const inet_address &o) const
{
    return o < *this;
}

bool chen::inet_address::operator<=(const inet_address &o) const
{
    return (this->_addr == o._addr) ? this->_port <= o._port : this->_addr <= o._addr;
}

bool chen::inet_address::operator>=(const inet_address &o) const
{
    return o <= *this;
}