/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_address.hpp>
#include <chen/base/str.hpp>

// -----------------------------------------------------------------------------
// address
chen::net::address::address(Type type)
{
    this->assign(type);
}

chen::net::address::address(version4 v4)
{
    this->assign(v4);
}

chen::net::address::address(version6 v6)
{
    this->assign(v6);
}

chen::net::address::address(const std::string &addr)
{
    this->assign(addr);
}

chen::net::address::address(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::net::address::address(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::net::address::address(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::net::address::address(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

// assignment
void chen::net::address::assign(Type type)
{
    this->_type = type;

    switch (this->_type)
    {
        case Type::IPv4:
            this->_impl.v4.assign();
            break;

        case Type::IPv6:
            this->_impl.v6.assign();
            break;
    }
}

void chen::net::address::assign(version4 v4)
{
    this->_type    = Type::IPv4;
    this->_impl.v4 = v4;
}

void chen::net::address::assign(version6 v6)
{
    this->_type    = Type::IPv6;
    this->_impl.v6 = v6;
}

void chen::net::address::assign(const std::string &addr)
{
    switch (address::detect(addr))
    {
        case Type::IPv4:
            this->_type = Type::IPv4;
            this->_impl.v4.assign(addr);
            break;

        case Type::IPv6:
            this->_type = Type::IPv6;
            this->_impl.v6.assign(addr);
            break;
    }
}

void chen::net::address::assign(const std::string &addr, std::uint8_t cidr)
{
    switch (address::detect(addr))
    {
        case Type::IPv4:
            this->_type = Type::IPv4;
            this->_impl.v4.assign(addr, cidr);
            break;

        case Type::IPv6:
            this->_type = Type::IPv6;
            this->_impl.v6.assign(addr, cidr);
            break;
    }
}

void chen::net::address::assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    switch (address::detect(addr))
    {
        case Type::IPv4:
            this->_type = Type::IPv4;
            this->_impl.v4.assign(addr, cidr);
            break;

        case Type::IPv6:
            this->_type = Type::IPv6;
            this->_impl.v6.assign(addr, cidr, scope);
            break;
    }
}

void chen::net::address::assign(const std::string &addr, const std::string &mask)
{
    switch (address::detect(addr))
    {
        case Type::IPv4:
            this->_type = Type::IPv4;
            this->_impl.v4.assign(addr, mask);
            break;

        case Type::IPv6:
            this->_type = Type::IPv6;
            this->_impl.v6.assign(addr, mask);
            break;
    }
}

void chen::net::address::assign(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    switch (address::detect(addr))
    {
        case Type::IPv4:
            this->_type = Type::IPv4;
            this->_impl.v4.assign(addr, mask);
            break;

        case Type::IPv6:
            this->_type = Type::IPv6;
            this->_impl.v6.assign(addr, mask, scope);
            break;
    }
}

chen::net::address& chen::net::address::operator=(Type type)
{
    this->assign(type);
    return *this;
}

chen::net::address& chen::net::address::operator=(version4 v4)
{
    this->assign(v4);
    return *this;
}

chen::net::address& chen::net::address::operator=(version6 v6)
{
    this->assign(v6);
    return *this;
}

chen::net::address& chen::net::address::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

// detect
chen::net::address::Type chen::net::address::type() const
{
    return this->_type;
}

bool chen::net::address::isIPv4() const
{
    return this->_type == Type::IPv4;
}

bool chen::net::address::isIPv6() const
{
    return this->_type == Type::IPv6;
}

const chen::net::version4& chen::net::address::v4() const
{
    return this->_impl.v4;
}

const chen::net::version6& chen::net::address::v6() const
{
    return this->_impl.v6;
}

chen::net::version4& chen::net::address::v4()
{
    return this->_impl.v4;
}

chen::net::version6& chen::net::address::v6()
{
    return this->_impl.v6;
}

// representation
std::string chen::net::address::str(bool cidr) const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.str(cidr);

        case Type::IPv6:
            return this->_impl.v6.str(cidr);
    }
}

std::vector<std::uint8_t> chen::net::address::bytes() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.bytes();

        case Type::IPv6:
            return this->_impl.v6.bytes();
    }
}

std::uint8_t chen::net::address::cidr() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.cidr();

        case Type::IPv6:
            return this->_impl.v6.cidr();
    }
}

void chen::net::address::cidr(std::uint8_t value)
{
    switch (this->_type)
    {
        case Type::IPv4:
            this->_impl.v4.cidr(value);
            break;

        case Type::IPv6:
            this->_impl.v6.cidr(value);
            break;
    }
}

std::uint32_t chen::net::address::scope() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return 0;

        case Type::IPv6:
            return this->_impl.v6.scope();
    }
}

void chen::net::address::scope(std::uint32_t value)
{
    switch (this->_type)
    {
        case Type::IPv4:
            break;

        case Type::IPv6:
            this->_impl.v6.scope(value);
            break;
    }
}

// network
chen::net::address chen::net::address::network() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.network());

        case Type::IPv6:
            return address(this->_impl.v6.network());
    }
}

chen::net::address chen::net::address::minhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.minhost());

        case Type::IPv6:
            return address(this->_impl.v6.minhost());
    }
}

chen::net::address chen::net::address::maxhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.maxhost());

        case Type::IPv6:
            return address(this->_impl.v6.maxhost());
    }
}

// special
bool chen::net::address::isUnspecified() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return !this->_impl.v4.addr();

        case Type::IPv6:
            return this->_impl.v6.isUnspecified();
    }
}

bool chen::net::address::isLoopback() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.isLoopback();

        case Type::IPv6:
            return this->_impl.v6.isLoopback();
    }
}

bool chen::net::address::isMulticast() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.isMulticast();

        case Type::IPv6:
            return this->_impl.v6.isMulticast();
    }
}

// operator
bool chen::net::address::operator==(const address &o) const
{
    if (this->_type != o._type)
        return false;

    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4 == o._impl.v4;

        case Type::IPv6:
            return this->_impl.v6 == o._impl.v6;
    }
}

bool chen::net::address::operator!=(const address &o) const
{
    return !(*this == o);
}

bool chen::net::address::operator<(const address &o) const
{
    if (this->_type != o._type)
        return false;

    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4 < o._impl.v4;

        case Type::IPv6:
            return this->_impl.v6 < o._impl.v6;
    }
}

bool chen::net::address::operator>(const address &o) const
{
    return o < *this;
}

bool chen::net::address::operator<=(const address &o) const
{
    if (this->_type != o._type)
        return false;

    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4 <= o._impl.v4;

        case Type::IPv6:
            return this->_impl.v6 <= o._impl.v6;
    }
}

bool chen::net::address::operator>=(const address &o) const
{
    return o <= *this;
}

// common
chen::net::address chen::net::address::any(Type type)
{
    address ret;

    switch (type)
    {
        case Type::IPv4:
            ret._type = Type::IPv4;
            ret._impl.v4.assign();
            break;

        case Type::IPv6:
            ret._type = Type::IPv6;
            ret._impl.v6.assign();
            break;
    }

    return ret;
}

chen::net::address chen::net::address::loopback(Type type)
{
    address ret;

    switch (type)
    {
        case Type::IPv4:
            ret._type = Type::IPv4;
            ret._impl.v4.assign(0x7F000001);
            break;

        case Type::IPv6:
            ret._type = Type::IPv6;
            ret._impl.v6.assign(std::array<std::uint8_t, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01});
            break;
    }

    return ret;
}

// helper
chen::net::address::Type chen::net::address::detect(const std::string &addr)
{
    return str::contain(addr, ":") ? Type::IPv6 : Type::IPv4;
}