/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_address.hpp>
#include <chen/base/str.hpp>

// -----------------------------------------------------------------------------
// ip_address
chen::ip_address::ip_address(std::nullptr_t) : _type(Type::None)
{
}

chen::ip_address::ip_address(Type type)
{
    this->assign(type);
}

chen::ip_address::ip_address(const ip_version4 &v4)
{
    this->assign(v4);
}

chen::ip_address::ip_address(const ip_version6 &v6)
{
    this->assign(v6);
}

chen::ip_address::ip_address(const char *addr) : ip_address(std::string(addr))
{
}

chen::ip_address::ip_address(const std::string &addr)
{
    this->assign(addr);
}

chen::ip_address::ip_address(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip_address::ip_address(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::ip_address::ip_address(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip_address::ip_address(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

// assignment
void chen::ip_address::assign(std::nullptr_t)
{
    this->_type = Type::None;
}

void chen::ip_address::assign(Type type)
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

void chen::ip_address::assign(const ip_version4 &v4)
{
    this->_type    = Type::IPv4;
    this->_impl.v4 = v4;
}

void chen::ip_address::assign(const ip_version6 &v6)
{
    this->_type    = Type::IPv6;
    this->_impl.v6 = v6;
}

void chen::ip_address::assign(const std::string &addr)
{
    switch (ip_address::detect(addr))
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

void chen::ip_address::assign(const std::string &addr, std::uint8_t cidr)
{
    switch (ip_address::detect(addr))
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

void chen::ip_address::assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    switch (ip_address::detect(addr))
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

void chen::ip_address::assign(const std::string &addr, const std::string &mask)
{
    switch (ip_address::detect(addr))
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

void chen::ip_address::assign(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    switch (ip_address::detect(addr))
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

chen::ip_address& chen::ip_address::operator=(std::nullptr_t)
{
    this->assign(nullptr);
    return *this;
}

chen::ip_address& chen::ip_address::operator=(Type type)
{
    this->assign(type);
    return *this;
}

chen::ip_address& chen::ip_address::operator=(const ip_version4 &v4)
{
    this->assign(v4);
    return *this;
}

chen::ip_address& chen::ip_address::operator=(const ip_version6 &v6)
{
    this->assign(v6);
    return *this;
}

chen::ip_address& chen::ip_address::operator=(const char *addr)
{
    return *this = std::string(addr);
}

chen::ip_address& chen::ip_address::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

// detect
chen::ip_address::Type chen::ip_address::type() const
{
    return this->_type;
}

bool chen::ip_address::isIPv4() const
{
    return this->_type == Type::IPv4;
}

bool chen::ip_address::isIPv6() const
{
    return this->_type == Type::IPv6;
}

const chen::ip_version4& chen::ip_address::v4() const
{
    return this->_impl.v4;
}

const chen::ip_version6& chen::ip_address::v6() const
{
    return this->_impl.v6;
}

chen::ip_version4& chen::ip_address::v4()
{
    return this->_impl.v4;
}

chen::ip_version6& chen::ip_address::v6()
{
    return this->_impl.v6;
}

// representation
std::string chen::ip_address::str(bool cidr, bool scope) const
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

std::vector<std::uint8_t> chen::ip_address::bytes() const
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

bool chen::ip_address::empty() const
{
    return this->_type == Type::None;
}

chen::ip_address::operator bool() const
{
    return !this->empty();
}

std::uint8_t chen::ip_address::cidr() const
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

void chen::ip_address::cidr(std::uint8_t value)
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

std::uint32_t chen::ip_address::scope() const
{
    return this->isIPv6() ? this->_impl.v6.scope() : 0;
}

void chen::ip_address::scope(std::uint32_t value)
{
    if (this->isIPv6())
        this->_impl.v6.scope(value);
}

// network
chen::ip_address chen::ip_address::network() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return ip_address(this->_impl.v4.network());

        case Type::IPv6:
            return ip_address(this->_impl.v6.network());

        default:
            return nullptr;
    }
}

chen::ip_address chen::ip_address::minhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return ip_address(this->_impl.v4.minhost());

        case Type::IPv6:
            return ip_address(this->_impl.v6.minhost());

        default:
            return nullptr;
    }
}

chen::ip_address chen::ip_address::maxhost() const
{
    switch (this->_type)
    {
        case Type::IPv4:
            return ip_address(this->_impl.v4.maxhost());

        case Type::IPv6:
            return ip_address(this->_impl.v6.maxhost());

        default:
            return nullptr;
    }
}

// special
bool chen::ip_address::isUnspecified() const
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

bool chen::ip_address::isLoopback() const
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

bool chen::ip_address::isMulticast() const
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
bool chen::ip_address::operator==(const ip_address &o) const
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
            return true;
    }
}

bool chen::ip_address::operator!=(const ip_address &o) const
{
    return !(*this == o);
}

bool chen::ip_address::operator<(const ip_address &o) const
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

bool chen::ip_address::operator>(const ip_address &o) const
{
    return o < *this;
}

bool chen::ip_address::operator<=(const ip_address &o) const
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
            return true;
    }
}

bool chen::ip_address::operator>=(const ip_address &o) const
{
    return o <= *this;
}

// common
chen::ip_address chen::ip_address::any(Type type)
{
    ip_address ret(nullptr);

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

chen::ip_address chen::ip_address::loopback(Type type)
{
    ip_address ret(nullptr);

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
chen::ip_address::Type chen::ip_address::detect(const std::string &addr)
{
    return str::contain(addr, ":") ? Type::IPv6 : Type::IPv4;
}