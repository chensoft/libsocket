/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_address.hpp>
#include <chen/base/str.hpp>

// -----------------------------------------------------------------------------
// address
chen::ip::address::address(std::nullptr_t) : _type(Type::None)
{
}

chen::ip::address::address(Type type)
{
    this->assign(type);
}

chen::ip::address::address(const version4 &v4)
{
    this->assign(v4);
}

chen::ip::address::address(const version6 &v6)
{
    this->assign(v6);
}

chen::ip::address::address(const char *addr) : address(std::string(addr))
{
}

chen::ip::address::address(const std::string &addr)
{
    this->assign(addr);
}

chen::ip::address::address(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip::address::address(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::ip::address::address(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip::address::address(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

// assignment
void chen::ip::address::assign(std::nullptr_t)
{
    this->_type = Type::None;
}

void chen::ip::address::assign(Type type)
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

        default:
            break;
    }
}

void chen::ip::address::assign(const version4 &v4)
{
    this->_type    = Type::IPv4;
    this->_impl.v4 = v4;
}

void chen::ip::address::assign(const version6 &v6)
{
    this->_type    = Type::IPv6;
    this->_impl.v6 = v6;
}

void chen::ip::address::assign(const std::string &addr)
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

        default:
            break;
    }
}

void chen::ip::address::assign(const std::string &addr, std::uint8_t cidr)
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

        default:
            break;
    }
}

void chen::ip::address::assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
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

        default:
            break;
    }
}

void chen::ip::address::assign(const std::string &addr, const std::string &mask)
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

        default:
            break;
    }
}

void chen::ip::address::assign(const std::string &addr, const std::string &mask, std::uint32_t scope)
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

        default:
            break;
    }
}

chen::ip::address& chen::ip::address::operator=(std::nullptr_t)
{
    this->assign(nullptr);
    return *this;
}

chen::ip::address& chen::ip::address::operator=(Type type)
{
    this->assign(type);
    return *this;
}

chen::ip::address& chen::ip::address::operator=(const version4 &v4)
{
    this->assign(v4);
    return *this;
}

chen::ip::address& chen::ip::address::operator=(const version6 &v6)
{
    this->assign(v6);
    return *this;
}

chen::ip::address& chen::ip::address::operator=(const char *addr)
{
    return *this = std::string(addr);
}

chen::ip::address& chen::ip::address::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

// detect
chen::ip::address::Type chen::ip::address::type() const
{
    return this->_type;
}

bool chen::ip::address::isIPv4() const
{
    return this->_type == Type::IPv4;
}

bool chen::ip::address::isIPv6() const
{
    return this->_type == Type::IPv6;
}

const chen::ip::version4& chen::ip::address::v4() const
{
    return this->_impl.v4;
}

const chen::ip::version6& chen::ip::address::v6() const
{
    return this->_impl.v6;
}

chen::ip::version4& chen::ip::address::v4()
{
    return this->_impl.v4;
}

chen::ip::version6& chen::ip::address::v6()
{
    return this->_impl.v6;
}

// representation
std::string chen::ip::address::str(bool cidr, bool scope) const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.str(cidr);

        case Type::IPv6:
            return this->_impl.v6.str(cidr, scope);

        default:
            return "";
    }
}

std::vector<std::uint8_t> chen::ip::address::bytes() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.bytes();

        case Type::IPv6:
            return this->_impl.v6.bytes();

        default:
            return {};
    }
}

bool chen::ip::address::empty() const
{
    return this->_type == Type::None;
}

chen::ip::address::operator bool() const
{
    return !this->empty();
}

std::uint8_t chen::ip::address::cidr() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.cidr();

        case Type::IPv6:
            return this->_impl.v6.cidr();

        default:
            return 0;
    }
}

void chen::ip::address::cidr(std::uint8_t value)
{
    switch (this->_type)
    {
        case Type::IPv4:
            this->_impl.v4.cidr(value);
            break;

        case Type::IPv6:
            this->_impl.v6.cidr(value);
            break;

        default:
            break;
    }
}

std::uint32_t chen::ip::address::scope() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return 0;

        case Type::IPv6:
            return this->_impl.v6.scope();

        default:
            return 0;
    }
}

void chen::ip::address::scope(std::uint32_t value)
{
    switch (this->_type)
    {
        case Type::IPv4:
            break;

        case Type::IPv6:
            this->_impl.v6.scope(value);
            break;

        default:
            break;
    }
}

// network
chen::ip::address chen::ip::address::network() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.network());

        case Type::IPv6:
            return address(this->_impl.v6.network());

        default:
            return address(nullptr);
    }
}

chen::ip::address chen::ip::address::minhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.minhost());

        case Type::IPv6:
            return address(this->_impl.v6.minhost());

        default:
            return address(nullptr);
    }
}

chen::ip::address chen::ip::address::maxhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return address(this->_impl.v4.maxhost());

        case Type::IPv6:
            return address(this->_impl.v6.maxhost());

        default:
            return address(nullptr);
    }
}

// special
bool chen::ip::address::isUnspecified() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return !this->_impl.v4.addr();

        case Type::IPv6:
            return this->_impl.v6.isUnspecified();

        default:
            return false;
    }
}

bool chen::ip::address::isLoopback() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.isLoopback();

        case Type::IPv6:
            return this->_impl.v6.isLoopback();

        default:
            return false;
    }
}

bool chen::ip::address::isMulticast() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4.isMulticast();

        case Type::IPv6:
            return this->_impl.v6.isMulticast();

        default:
            return false;
    }
}

// operator
bool chen::ip::address::operator==(const address &o) const
{
    if (this->_type != o._type)
        return false;

    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4 == o._impl.v4;

        case Type::IPv6:
            return this->_impl.v6 == o._impl.v6;

        default:
            return false;
    }
}

bool chen::ip::address::operator!=(const address &o) const
{
    return !(*this == o);
}

bool chen::ip::address::operator<(const address &o) const
{
    if (this->_type != o._type)
        return false;

    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4 < o._impl.v4;

        case Type::IPv6:
            return this->_impl.v6 < o._impl.v6;

        default:
            return false;
    }
}

bool chen::ip::address::operator>(const address &o) const
{
    return o < *this;
}

bool chen::ip::address::operator<=(const address &o) const
{
    if (this->_type != o._type)
        return false;

    switch (this->_type)
    {
        case Type::IPv4:
            return this->_impl.v4 <= o._impl.v4;

        case Type::IPv6:
            return this->_impl.v6 <= o._impl.v6;

        default:
            return false;
    }
}

bool chen::ip::address::operator>=(const address &o) const
{
    return o <= *this;
}

// common
chen::ip::address chen::ip::address::any(Type type)
{
    address ret(nullptr);

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

        default:
            break;
    }

    return ret;
}

chen::ip::address chen::ip::address::loopback(Type type)
{
    address ret(nullptr);

    switch (type)
    {
        case Type::IPv4:
            ret._type = Type::IPv4;
            ret._impl.v4.assign(0x7F000001);
            break;

        case Type::IPv6:
        {
            std::uint8_t bytes[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01};
            ret._type = Type::IPv6;
            ret._impl.v6.assign(bytes);
            break;
        }

        default:
            break;
    }

    return ret;
}

// helper
chen::ip::address::Type chen::ip::address::detect(const std::string &addr)
{
    return str::contain(addr, ":") ? Type::IPv6 : Type::IPv4;
}