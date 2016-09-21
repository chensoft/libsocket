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
chen::net::endpoint::endpoint(std::nullptr_t) : _addr(nullptr)
{
}

chen::net::endpoint::endpoint(const ip::address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

chen::net::endpoint::endpoint(const std::string &mixed)
{
    auto ret = resolver::split(mixed);
    this->_addr = ret.first;
    this->_port = ret.second;
}

chen::net::endpoint::endpoint(const char *mixed) : endpoint(std::string(mixed))
{
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

// override
struct ::sockaddr_storage chen::net::endpoint::get() const noexcept
{
    struct ::sockaddr_storage out{};

    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
        {
            auto in = (struct ::sockaddr_in*)&out;

            in->sin_family      = AF_INET;
            in->sin_port        = chen::num::swap(this->_port);
            in->sin_addr.s_addr = chen::num::swap(this->_addr.v4().addr());
        }
            break;

        case ip::address::Type::IPv6:
        {
            auto in = (struct ::sockaddr_in6*)&out;

            in->sin6_family   = AF_INET6;
            in->sin6_port     = chen::num::swap(this->_port);
            in->sin6_scope_id = this->_addr.v6().scope();

            ::memcpy(in->sin6_addr.s6_addr, this->_addr.v6().addr().data(), 16);
        }
            break;

        default:
            break;
    }

    return out;
}

void chen::net::endpoint::set(const struct ::sockaddr *ptr) noexcept
{
    switch (ptr->sa_family)
    {
        case AF_INET:
        {
            auto in = (struct ::sockaddr_in*)&ptr;
            this->_addr = num::swap(in->sin_addr.s_addr);
            this->_port = num::swap(in->sin_port);
        }
            break;

        case AF_INET6:
        {
            auto in = (struct ::sockaddr_in6*)&ptr;
            this->_addr = ip::version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id);
            this->_port = num::swap(in->sin6_port);
        }
            break;

        default:
            this->_addr = nullptr;
            this->_port = 0;
    }
}

socklen_t chen::net::endpoint::len() const noexcept
{
    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
            return sizeof(struct ::sockaddr_in);

        case ip::address::Type::IPv6:
            return sizeof(struct ::sockaddr_in6);

        default:
            return 0;
    }
}