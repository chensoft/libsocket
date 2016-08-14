/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_address.hpp>
#include <chen/base/str.hpp>

using namespace chen;
using namespace chen::net;

// -----------------------------------------------------------------------------
// address
address::address(Type type)
{
    this->assign(type);
}

address::address(version4 v4)
{
    this->assign(v4);
}

address::address(version6 v6)
{
    this->assign(v6);
}

address::address(const std::string &addr)
{
    this->assign(addr);
}

address::address(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

address::address(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

address::address(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

address::address(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

// assignment
void address::assign(Type type)
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

void address::assign(version4 v4)
{
    this->_type    = Type::IPv4;
    this->_impl.v4 = v4;
}

void address::assign(version6 v6)
{
    this->_type    = Type::IPv6;
    this->_impl.v6 = v6;
}

void address::assign(const std::string &addr)
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

void address::assign(const std::string &addr, std::uint8_t cidr)
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

void address::assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
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

void address::assign(const std::string &addr, const std::string &mask)
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

void address::assign(const std::string &addr, const std::string &mask, std::uint32_t scope)
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

address& address::operator=(Type type)
{
    this->assign(type);
    return *this;
}

address& address::operator=(version4 v4)
{
    this->assign(v4);
    return *this;
}

address& address::operator=(version6 v6)
{
    this->assign(v6);
    return *this;
}

address& address::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

// detect
address::Type address::type() const
{
    return this->_type;
}

bool address::isIPv4() const
{
    return this->_type == Type::IPv4;
}

bool address::isIPv6() const
{
    return this->_type == Type::IPv6;
}

const version4& address::v4() const
{
    return this->_impl.v4;
}

const version6& address::v6() const
{
    return this->_impl.v6;
}

version4& address::v4()
{
    return this->_impl.v4;
}

version6& address::v6()
{
    return this->_impl.v6;
}

// representation
std::string address::str(bool cidr) const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.str(cidr);

        case Type::IPv6:
            return this->_impl.v6.str(cidr);
    }
}

std::vector<std::uint8_t> address::bytes() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.bytes();

        case Type::IPv6:
            return this->_impl.v6.bytes();
    }
}

std::uint8_t address::cidr() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.cidr();

        case Type::IPv6:
            return this->_impl.v6.cidr();
    }
}

void address::cidr(std::uint8_t value)
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

std::uint32_t address::scope() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return 0;

        case Type::IPv6:
            return this->_impl.v6.scope();
    }
}

void address::scope(std::uint32_t value)
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
address address::network() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.network());

        case Type::IPv6:
            return address(this->_impl.v6.network());
    }
}

address address::minhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.minhost());

        case Type::IPv6:
            return address(this->_impl.v6.minhost());
    }
}

address address::maxhost() const
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
bool address::isUnspecified() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return !this->_impl.v4.addr();

        case Type::IPv6:
            return this->_impl.v6.isUnspecified();
    }
}

bool address::isLoopback() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.isLoopback();

        case Type::IPv6:
            return this->_impl.v6.isLoopback();
    }
}

bool address::isMulticast() const
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
bool address::operator==(const address &o) const
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

bool address::operator!=(const address &o) const
{
    return !(*this == o);
}

bool address::operator<(const address &o) const
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

bool address::operator>(const address &o) const
{
    return o < *this;
}

bool address::operator<=(const address &o) const
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

bool address::operator>=(const address &o) const
{
    return o <= *this;
}

// common
address address::any(Type type)
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

address address::loopback(Type type)
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
address::Type address::detect(const std::string &addr)
{
    return str::contain(addr, ":") ? Type::IPv6 : Type::IPv4;
}