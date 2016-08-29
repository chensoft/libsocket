/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <chen/base/num.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// endpoint
chen::net::endpoint::endpoint(std::nullptr_t) : _addr(nullptr)
{
}

chen::net::endpoint::endpoint(const address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

chen::net::endpoint::endpoint(std::pair<std::string, std::uint16_t> pair) : _addr(pair.first), _port(pair.second)
{
}

chen::net::endpoint::endpoint(const std::string &mixed) : endpoint(resolver::split(mixed))
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

// conversion
chen::net::endpoint chen::net::endpoint::toEndpoint(const struct sockaddr *addr)
{
    switch (addr->sa_family)
    {
        case AF_INET:
        {
            auto in = (struct sockaddr_in*)addr;
            return endpoint(address(num::swap(in->sin_addr.s_addr)),
                            num::swap(in->sin_port));
        }

        case AF_INET6:
        {
            auto in = (struct sockaddr_in6*)addr;
            return endpoint(address(version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id)),
                            num::swap(in->sin6_port));
        }

        default:
            return nullptr;
    }
}

chen::net::endpoint chen::net::endpoint::toEndpoint(const struct sockaddr_in *addr)
{
    return endpoint::toEndpoint((const struct sockaddr*)addr);
}

chen::net::endpoint chen::net::endpoint::toEndpoint(const struct sockaddr_in6 *addr)
{
    return endpoint::toEndpoint((const struct sockaddr*)addr);
}

chen::net::endpoint chen::net::endpoint::toEndpoint(const struct sockaddr_storage *addr)
{
    return endpoint::toEndpoint((const struct sockaddr*)addr);
}

void chen::net::endpoint::toAddress(const endpoint &ep, struct sockaddr_storage &out, socklen_t &len)
{
    switch (ep.addr().type())
    {
        case chen::net::address::Type::IPv4:
        {
            auto &v4 = ep.addr().v4();
            auto  in = (struct sockaddr_in*)&out;

            in->sin_family      = AF_INET;
            in->sin_port        = chen::num::swap(ep.port());
            in->sin_addr.s_addr = chen::num::swap(v4.addr());

            len = sizeof(struct sockaddr_in);

            break;
        }

        case chen::net::address::Type::IPv6:
        {
            auto &v6 = ep.addr().v6();
            auto  in = (struct sockaddr_in6*)&out;

            in->sin6_family   = AF_INET6;
            in->sin6_port     = chen::num::swap(ep.port());
            in->sin6_scope_id = v6.scope();

            ::memcpy(in->sin6_addr.s6_addr, v6.addr().data(), 16);

            len = sizeof(struct sockaddr_in6);

            break;
        }

        default:
            break;
    }
}