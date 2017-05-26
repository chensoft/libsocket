/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/inet/inet_adapter.hpp"
#include "chen/base/num.hpp"
#include "chen/base/str.hpp"
#include <algorithm>
#include <cstring>
#include <cctype>

// -----------------------------------------------------------------------------
// ip_version
std::uint8_t chen::ip_version::cidr() const
{
    // @see rfc1519
    return this->_cidr;
}

void chen::ip_version::cidr(std::uint8_t value)
{
    // @see rfc1519
    this->_cidr = value;
}


// -----------------------------------------------------------------------------
// ip_version4
chen::ip_version4::ip_version4(const std::string &addr)
{
    this->assign(addr);
}

chen::ip_version4::ip_version4(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip_version4::ip_version4(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip_version4::ip_version4(std::uint32_t addr)
{
    this->assign(addr);
}

chen::ip_version4::ip_version4(std::uint32_t addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip_version4::ip_version4(std::uint32_t addr, const std::string &mask)
{
    this->assign(addr, mask);
}

// assignment
void chen::ip_version4::assign()
{
    this->_addr = 0;
    this->_cidr = 32;
}

void chen::ip_version4::assign(const std::string &addr)
{
    this->_addr = ip_version4::toInteger(addr, &this->_cidr);
}

void chen::ip_version4::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = ip_version4::toInteger(addr);
    this->_cidr = cidr;

    if (this->_cidr > 32)
        throw std::runtime_error("ipv4: CIDR prefix must less than 32");
}

void chen::ip_version4::assign(const std::string &addr, const std::string &mask)
{
    this->_addr = ip_version4::toInteger(addr);
    this->_cidr = ip_version4::toCIDR(mask);
}

void chen::ip_version4::assign(std::uint32_t addr)
{
    this->_addr = addr;
    this->_cidr = 32;
}

void chen::ip_version4::assign(std::uint32_t addr, std::uint8_t cidr)
{
    this->_addr = addr;
    this->_cidr = cidr;

    if (this->_cidr > 32)
        throw std::runtime_error("ipv4: CIDR prefix must less than 32");
}

void chen::ip_version4::assign(std::uint32_t addr, const std::string &mask)
{
    this->_addr = addr;
    this->_cidr = ip_version4::toCIDR(mask);
}

chen::ip_version4& chen::ip_version4::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

chen::ip_version4& chen::ip_version4::operator=(std::uint32_t addr)
{
    this->assign(addr);
    return *this;
}

// representation
std::string chen::ip_version4::str(bool cidr) const
{
    return !cidr ? ip_version4::toString(this->_addr) : ip_version4::toString(this->_addr, this->_cidr);
}

std::vector<std::uint8_t> chen::ip_version4::bytes() const
{
    return std::vector<std::uint8_t>{
            static_cast<std::uint8_t>(this->_addr >> 24 & 0xff),
            static_cast<std::uint8_t>(this->_addr >> 16 & 0xff),
            static_cast<std::uint8_t>(this->_addr >> 8 & 0xff),
            static_cast<std::uint8_t>(this->_addr & 0xff)
    };
}

std::uint32_t chen::ip_version4::addr() const
{
    // @see rfc791
    return this->_addr;
}

void chen::ip_version4::addr(std::uint32_t value)
{
    // @see rfc791
    this->_addr = value;
}

// network
std::uint32_t chen::ip_version4::netmask() const
{
    // @see rfc1878
    return 0xffffffffu << (32 - this->_cidr);
}

std::uint32_t chen::ip_version4::wildcard() const
{
    // @link https://en.wikipedia.org/wiki/Wildcard_mask
    return ~this->netmask();
}

chen::ip_version4 chen::ip_version4::network() const
{
    return ip_version4(this->_addr & this->netmask(), this->_cidr);
}

chen::ip_version4 chen::ip_version4::minhost() const
{
    return ip_version4((this->_addr & this->netmask()) | 0x00000001, this->_cidr);
}

chen::ip_version4 chen::ip_version4::maxhost() const
{
    return ip_version4((this->_addr | this->wildcard()) & 0xfffffffe, this->_cidr);
}

chen::ip_version4 chen::ip_version4::broadcast() const
{
    return ip_version4(this->_addr | this->wildcard(), this->_cidr);
}

std::size_t chen::ip_version4::hosts() const
{
    return this->maxhost().addr() - this->minhost().addr() + 1;
}

// special
bool chen::ip_version4::isReserved() const
{
    // @link https://en.wikipedia.org/wiki/Reserved_IP_addresses

    // 0.0.0.0/8
    // @see rfc1700, page 4
    if ((this->_addr & 0xff000000) == 0)
        return true;

    // loopback
    if (this->isLoopback())
        return true;

    // link-local
    if (this->isLinkLocal())
        return true;

    // 192.0.0.0/24, it's reserved, not private network according to its rfc
    // @see rfc5736, section 2
    if ((this->_addr & 0xffffff00) == 0xc0000000)
        return true;

    // 192.0.2.0/24
    // @see rfc5737, section 3
    if ((this->_addr & 0xffffff00) == 0xc0000200)
        return true;

    // 192.88.99.0/24
    // @see rfc3068, section 2.3
    if ((this->_addr & 0xffffff00) == 0xc0586300)
        return true;

    // 198.51.100.0/24
    // @see rfc5737, section 3
    if ((this->_addr & 0xffffff00) == 0xc6336400)
        return true;

    // 203.0.113.0/24
    // @see rfc5737, section 3
    if ((this->_addr & 0xffffff00) == 0xcb007100)
        return true;

    // class D & E
    if (this->isClassD() || this->isClassE())
        return true;

    // private address is also a reserved address
    return this->isPrivate();
}

bool chen::ip_version4::isPrivate() const
{
    // 10.0.0.0/8
    // @see rfc1918, section 3
    if ((this->_addr & 0xff000000) == 0x0a000000)
        return true;

    // 100.64.0.0/10
    // @see rfc6598, section 7
    if ((this->_addr & 0xffc00000) == 0x64400000)
        return true;

    // 172.16.0.0/12
    // @see rfc1918, section 3
    if ((this->_addr & 0xfff00000) == 0xac100000)
        return true;

    // 192.168.0.0/16
    // @see rfc1918, section 3
    if ((this->_addr & 0xffff0000) == 0xc0a80000)
        return true;

    // 198.18.0.0/15
    // @see rfc2544
    return (this->_addr & 0xfffe0000) == 0xc6120000;
}

bool chen::ip_version4::isLoopback() const
{
    // 127.0.0.0/8
    // @see rfc990 & rfc6890, section 2.2.2
    return (this->_addr & 0xff000000) == 0x7f000000;
}

bool chen::ip_version4::isLinkLocal() const
{
    // 169.254.0.0/16
    // @see rfc3927
    return (this->_addr & 0xffff0000) == 0xa9fe0000;
}

bool chen::ip_version4::isMulticast() const
{
    return this->isClassD();
}

bool chen::ip_version4::isBroadcast() const
{
    // host bits are 1
    return (this->_addr | this->wildcard()) == this->_addr;
}

// classful
bool chen::ip_version4::isClassA() const
{
    // leading: 0, network: 8, range: 0.0.0.0 ~ 127.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0x80000000) == 0;
}

bool chen::ip_version4::isClassB() const
{
    // leading: 10, network: 16, range: 128.0.0.0 ~ 191.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0xc0000000) == 0x80000000;
}

bool chen::ip_version4::isClassC() const
{
    // leading: 110, network: 24, range: 192.0.0.0 ~ 223.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0xe0000000) == 0xc0000000;
}

bool chen::ip_version4::isClassD() const
{
    // leading: 1110, range: 224.0.0.0 ~ 239.255.255.255
    // @see rfc1112, section 4
    return (this->_addr & 0xf0000000) == 0xe0000000;
}

bool chen::ip_version4::isClassE() const
{
    // leading: 1111, range: 240.0.0.0 ~ 255.255.255.255
    // @see rfc1112, section 4
    return (this->_addr & 0xf0000000) == 0xf0000000;
}

// operator
bool chen::ip_version4::operator==(const ip_version4 &o) const
{
    return (this->_addr == o._addr) && (this->_cidr == o._cidr);
}

bool chen::ip_version4::operator!=(const ip_version4 &o) const
{
    return !(*this == o);
}

bool chen::ip_version4::operator<(const ip_version4 &o) const
{
    return (this->_addr == o._addr) ? this->_cidr < o._cidr : this->_addr < o._addr;
}

bool chen::ip_version4::operator>(const ip_version4 &o) const
{
    return o < *this;
}

bool chen::ip_version4::operator<=(const ip_version4 &o) const
{
    return (this->_addr == o._addr) ? this->_cidr <= o._cidr : this->_addr <= o._addr;
}

bool chen::ip_version4::operator>=(const ip_version4 &o) const
{
    return o <= *this;
}

// convert
std::string chen::ip_version4::toString(std::uint32_t addr)
{
    return str::format("%u.%u.%u.%u",
                       addr >> 24 & 0xff,
                       addr >> 16 & 0xff,
                       addr >> 8 & 0xff,
                       addr & 0xff);
}

std::string chen::ip_version4::toString(std::uint32_t addr, std::uint8_t cidr)
{
    return ip_version4::toString(addr) + "/" + num::str(cidr);
}

std::uint32_t chen::ip_version4::toInteger(const std::string &addr)
{
    return ip_version4::toInteger(addr, nullptr);
}

std::uint32_t chen::ip_version4::toInteger(const std::string &addr, std::uint8_t *cidr)
{
    auto cur = addr.begin();
    auto end = addr.end();

    std::uint32_t idx   = 0;
    std::uint32_t num[] = {0, 0, 0, 0};

    for (int i = 0; (i < 4) && (cur != end) && (*cur != '/'); ++i, ++idx)
    {
        // check character
        if (!std::isdigit(*cur))
            throw std::runtime_error("ipv4: addr format is wrong");

        // collect digits
        do
        {
            num[i] = num[i] * 10 + (*cur++ - '0');
        } while ((cur != end) && std::isdigit(*cur));

        if ((cur != end) && (*cur == '.'))
            ++cur;

        // check if valid
        if (num[i] > 0xff)
            throw std::runtime_error("ipv4: addr number must between 0 and 255");
    }

    // analyse the digits
    std::uint32_t val = 0;

    switch (idx)
    {
        case 1:
            // 127 -> 127.0.0.0, 127 treated as the high 8 bits
            val = num[0] << 24;
            break;

        case 2:
            // 127.1 -> 127.0.0.1, 1 treated as 24 bits
            val = (num[0] << 24) | num[1];
            break;

        case 3:
            // 1.2.3 -> 1.2.0.3, 3 treated as 16 bits
            val = (num[0] << 24) | (num[1] << 16) | num[2];
            break;

        case 4:
            // 8.8.8.8 -> 8.8.8.8
            val = (num[0] << 24) | (num[1] << 16) | (num[2] << 8) | num[3];
            break;

        default:
            break;
    }

    // CIDR notation
    if (cidr)
    {
        *cidr = 32;

        if ((cur != end) && (*cur == '/'))
        {
            std::uint32_t tmp = 0;

            while ((++cur != end) && std::isdigit(*cur))
                tmp = tmp * 10 + (*cur - '0');

            if (tmp > 32)
                throw std::runtime_error("ipv4: CIDR prefix must less than 32");

            *cidr = static_cast<uint8_t>(tmp);
        }
    }

    return val;
}

std::uint8_t chen::ip_version4::toCIDR(const std::string &mask)
{
    return ip_version4::toCIDR(ip_version4::toInteger(mask));
}

std::uint8_t chen::ip_version4::toCIDR(std::uint32_t mask)
{
    return static_cast<std::uint8_t>(num::bits(mask));
}


// -----------------------------------------------------------------------------
// ip_version6
chen::ip_version6::ip_version6(const std::string &addr)
{
    this->assign(addr);
}

chen::ip_version6::ip_version6(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip_version6::ip_version6(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::ip_version6::ip_version6(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip_version6::ip_version6(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

chen::ip_version6::ip_version6(const std::uint8_t addr[16])
{
    this->assign(addr);
}

chen::ip_version6::ip_version6(const std::uint8_t addr[16], std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip_version6::ip_version6(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::ip_version6::ip_version6(const std::uint8_t addr[16], const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip_version6::ip_version6(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

// assignment
void chen::ip_version6::assign()
{
    this->_addr.fill(0);
    this->_cidr  = 128;
    this->_scope = 0;
}

void chen::ip_version6::assign(const std::string &addr)
{
    this->_addr = ip_version6::toBytes(addr, &this->_cidr, &this->_scope);
}

void chen::ip_version6::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = ip_version6::toBytes(addr, nullptr, &this->_scope);
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw std::runtime_error("ipv6: CIDR prefix must less than 128");
}

void chen::ip_version6::assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->_addr  = ip_version6::toBytes(addr);
    this->_cidr  = cidr;
    this->_scope = scope;

    if (this->_cidr > 128)
        throw std::runtime_error("ipv6: CIDR prefix must less than 128");
}

void chen::ip_version6::assign(const std::string &addr, const std::string &mask)
{
    this->_addr = ip_version6::toBytes(addr, nullptr, &this->_scope);
    this->_cidr = ip_version6::toCIDR(mask);
}

void chen::ip_version6::assign(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->_addr  = ip_version6::toBytes(addr);
    this->_cidr  = ip_version6::toCIDR(mask);
    this->_scope = scope;
}

void chen::ip_version6::assign(const std::uint8_t addr[16])
{
    this->assign(addr, 128, 0);
}

void chen::ip_version6::assign(const std::uint8_t addr[16], std::uint8_t cidr)
{
    this->assign(addr, cidr, 0);
}

void chen::ip_version6::assign(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope)
{
    ::memcpy(this->_addr.data(), addr, 16);

    this->_cidr  = cidr;
    this->_scope = scope;

    if (this->_cidr > 128)
        throw std::runtime_error("ipv6: CIDR prefix must less than 128");
}

void chen::ip_version6::assign(const std::uint8_t addr[16], const std::string &mask)
{
    this->assign(addr, mask, 0);
}

void chen::ip_version6::assign(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope)
{
    ::memcpy(this->_addr.data(), addr, 16);

    this->_cidr  = ip_version6::toCIDR(mask);
    this->_scope = scope;
}

chen::ip_version6& chen::ip_version6::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

chen::ip_version6& chen::ip_version6::operator=(const std::uint8_t addr[16])
{
    this->assign(addr);
    return *this;
}

// representation
std::string chen::ip_version6::str(bool cidr) const
{
    return this->str(cidr, false);
}

std::string chen::ip_version6::str(bool cidr, bool scope) const
{
    if (cidr && scope)
        return ip_version6::toScope(this->_addr.data(), this->_cidr, this->_scope);
    else if (cidr)
        return ip_version6::toString(this->_addr.data(), this->_cidr);
    else if (scope)
        return ip_version6::toScope(this->_addr.data(), this->_scope);
    else
        return ip_version6::toString(this->_addr.data());
}

std::vector<std::uint8_t> chen::ip_version6::bytes() const
{
    return std::vector<std::uint8_t>(this->_addr.begin(), this->_addr.end());
}

std::string chen::ip_version6::expanded() const
{
    return ip_version6::toExpanded(this->_addr.data());
}

std::string chen::ip_version6::suppressed() const
{
    return ip_version6::toSuppressed(this->_addr.data());
}

std::string chen::ip_version6::compressed() const
{
    return ip_version6::toCompressed(this->_addr.data());
}

std::string chen::ip_version6::mixed() const
{
    return ip_version6::toMixed(this->_addr.data());
}

chen::ip_version4 chen::ip_version6::embedded() const
{
    // IPv4-compatible & IPv4-mapped address, @see rfc4291, section 2.5.5
    // IPv4-embedded address, @see rfc6052, section 2.2
    unsigned cidr = this->isIPv4Compatible() || this->isIPv4Mapped() ? 128 : this->_cidr;
    unsigned a = 0, b = 0, c = 0, d = 0;

    switch (cidr)
    {
        case 32:
            a = this->_addr[4];
            b = this->_addr[5];
            c = this->_addr[6];
            d = this->_addr[7];
            break;

        case 40:
            a = this->_addr[5];
            b = this->_addr[6];
            c = this->_addr[7];
            d = this->_addr[9];  // pass 'u'
            break;

        case 48:
            a = this->_addr[6];
            b = this->_addr[7];
            c = this->_addr[9];  // pass 'u'
            d = this->_addr[10];
            break;

        case 56:
            a = this->_addr[7];
            b = this->_addr[9];  // pass 'u'
            c = this->_addr[10];
            d = this->_addr[11];
            break;

        case 64:
            a = this->_addr[9];  // pass 'u'
            b = this->_addr[10];
            c = this->_addr[11];
            d = this->_addr[12];
            break;

        default:
            // IPv4-compatible & IPv4-mapped or IPv4-embedded with 96 bits prefix
            a = this->_addr[12];
            b = this->_addr[13];
            c = this->_addr[14];
            d = this->_addr[15];
            break;
    }

    return ip_version4(a * 256u * 256u * 256u +
                       b * 256u * 256u +
                       c * 256u +
                       d);
}

const std::array<std::uint8_t, 16>& chen::ip_version6::addr() const
{
    // @see rfc4291
    return this->_addr;
}

void chen::ip_version6::addr(const std::uint8_t value[16])
{
    // @see rfc4291
    ::memcpy(this->_addr.data(), value, 16);
}

std::uint32_t chen::ip_version6::scope() const
{
    // @see rfc4007
    return this->_scope;
}

void chen::ip_version6::scope(std::uint32_t value)
{
    // @see rfc4007
    this->_scope = value;
}

// network
std::array<std::uint8_t, 16> chen::ip_version6::netmask() const
{
    std::array<std::uint8_t, 16> ret{};

    auto len = this->_cidr / 8;
    auto mod = this->_cidr % 8;

    auto i = 0;

    for (; i < len; ++i)
        ret[i] = 0xff;

    if (mod)
        ret[i] = static_cast<std::uint8_t>(0xff << (8 - mod));

    return ret;
};

std::array<std::uint8_t, 16> chen::ip_version6::wildcard() const
{
    std::array<std::uint8_t, 16> ret{};

    auto len = this->_cidr / 8;
    auto mod = this->_cidr % 8;

    auto it = ret.begin() + len;

    if (mod)
    {
        ++it;
        ret[len] = static_cast<std::uint8_t>(~(0xff << (8 - mod)));
    }

    for (; it != ret.end(); ++it)
        *it = 0xff;

    return ret;
};

chen::ip_version6 chen::ip_version6::network() const
{
    std::uint8_t tmp[16]{};
    std::array<std::uint8_t, 16> mask = this->netmask();

    for (std::size_t i = 0, len = mask.size(); i < len; ++i)
        tmp[i] = this->_addr[i] & mask[i];

    return ip_version6(tmp, this->_cidr, this->_scope);
}

chen::ip_version6 chen::ip_version6::minhost() const
{
    // IPv6 host begins with 0
    return this->network();
}

chen::ip_version6 chen::ip_version6::maxhost() const
{
    // IPv6 host ends with 1
    std::uint8_t tmp[16]{};
    std::array<std::uint8_t, 16> mask = this->wildcard();

    for (std::size_t i = 0, len = mask.size(); i < len; ++i)
        tmp[i] = this->_addr[i] | mask[i];

    return ip_version6(tmp, this->_cidr, this->_scope);
}

// special
bool chen::ip_version6::isUnspecified() const
{
    // all bits are zero
    // @see rfc4291, section 2.5.2
    return std::all_of(this->_addr.begin(), this->_addr.end(), [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

bool chen::ip_version6::isLoopback() const
{
    // 0:0:0:0:0:0:0:1
    // @see rfc4291, section 2.5.3
    auto ret = std::all_of(this->_addr.begin(), this->_addr.begin() + 15, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });

    if (!ret)
        return false;

    return this->_addr[15] == 1;
}

bool chen::ip_version6::isGlobalUnicast() const
{
    // first 3 bits are 001
    // @see rfc3587, section 3
    return (this->_addr[0] & 0xe0) == 0x20;
}

bool chen::ip_version6::isLinkLocalUnicast() const
{
    // first 10 bits are 1111111010
    // @see rfc4291, section 2.5.6
    return ((this->_addr[0] == 0xfe) && ((this->_addr[1] & 0xc0) == 0x80));
}

bool chen::ip_version6::isSiteLocalUnicast() const
{
    // first 10 bits are 1111111011
    // @see rfc4291, section 2.5.7
    return ((this->_addr[0] == 0xfe) && ((this->_addr[1] & 0xc0) == 0xc0));
}

bool chen::ip_version6::isIPv4Compatible() const
{
    // first 96 bits are zero
    // @see rfc4291, section 2.5.5.1
    return std::all_of(this->_addr.begin(), this->_addr.begin() + 12, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

bool chen::ip_version6::isIPv4Mapped() const
{
    // first 80 bits are zero, next 16 bits are one
    // @see rfc4291, section 2.5.5.2
    auto ret = std::all_of(this->_addr.begin(), this->_addr.begin() + 10, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });

    if (!ret)
        return false;

    return (this->_addr[10] == 0xff) && (this->_addr[11] == 0xff);
}

bool chen::ip_version6::isMulticast() const
{
    // first 8 bits is 0xff
    // @see rfc4291, section 2.7
    return this->_addr[0] == 0xff;
}

// NAT64
bool chen::ip_version6::isIPv4EmbeddedWellKnown() const
{
    // "64:ff9b::/96"
    // @see rfc6052, section 2.1
    if ((this->_addr[0] != 0) || (this->_addr[1] != 0x64) || (this->_addr[2] != 0xff) || (this->_addr[3] != 0x9b))
        return false;

    return std::all_of(this->_addr.begin() + 4, this->_addr.begin() + 12, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

// operator
bool chen::ip_version6::operator==(const ip_version6 &o) const
{
    return (this->_addr == o._addr) && (this->_cidr == o._cidr) && (this->_scope == o._scope);
}

bool chen::ip_version6::operator!=(const ip_version6 &o) const
{
    return !(*this == o);
}

bool chen::ip_version6::operator<(const ip_version6 &o) const
{
    if (this->_addr == o._addr)
        return this->_cidr == o._cidr ? this->_scope < o._scope : this->_cidr < o._cidr;
    else
        return this->_addr < o._addr;
}

bool chen::ip_version6::operator>(const ip_version6 &o) const
{
    return o < *this;
}

bool chen::ip_version6::operator<=(const ip_version6 &o) const
{
    if (this->_addr == o._addr)
        return this->_cidr == o._cidr ? this->_scope <= o._scope : this->_cidr <= o._cidr;
    else
        return this->_addr <= o._addr;
}

bool chen::ip_version6::operator>=(const ip_version6 &o) const
{
    return o <= *this;
}

// convert
std::string chen::ip_version6::toString(const std::uint8_t addr[16])
{
    return ip_version6::toCompressed(addr);
}

std::string chen::ip_version6::toString(const std::uint8_t addr[16], std::uint8_t cidr)
{
    return ip_version6::toCompressed(addr) + "/" + num::str(cidr);
}

std::string chen::ip_version6::toScope(const std::uint8_t addr[16], std::uint32_t scope)
{
    return ip_version6::toCompressed(addr) + "%" + inet_adapter::scope(scope);
}

std::string chen::ip_version6::toScope(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope)
{
    return ip_version6::toCompressed(addr) + "%" + inet_adapter::scope(scope) + "/" + num::str(cidr);
}

std::string chen::ip_version6::toExpanded(const std::uint8_t addr[16])
{
    return str::format("%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                       (static_cast<unsigned>(addr[0]) << 8) + addr[1],
                       (static_cast<unsigned>(addr[2]) << 8) + addr[3],
                       (static_cast<unsigned>(addr[4]) << 8) + addr[5],
                       (static_cast<unsigned>(addr[6]) << 8) + addr[7],
                       (static_cast<unsigned>(addr[8]) << 8) + addr[9],
                       (static_cast<unsigned>(addr[10]) << 8) + addr[11],
                       (static_cast<unsigned>(addr[12]) << 8) + addr[13],
                       (static_cast<unsigned>(addr[14]) << 8) + addr[15]);
}

std::string chen::ip_version6::toSuppressed(const std::uint8_t addr[16])
{
    return str::format("%x:%x:%x:%x:%x:%x:%x:%x",
                       (static_cast<unsigned>(addr[0]) << 8) + addr[1],
                       (static_cast<unsigned>(addr[2]) << 8) + addr[3],
                       (static_cast<unsigned>(addr[4]) << 8) + addr[5],
                       (static_cast<unsigned>(addr[6]) << 8) + addr[7],
                       (static_cast<unsigned>(addr[8]) << 8) + addr[9],
                       (static_cast<unsigned>(addr[10]) << 8) + addr[11],
                       (static_cast<unsigned>(addr[12]) << 8) + addr[13],
                       (static_cast<unsigned>(addr[14]) << 8) + addr[15]);
}

std::string chen::ip_version6::toCompressed(const std::uint8_t addr[16])
{
    return ip_version6::compress(addr, addr + 16);
}

std::string chen::ip_version6::toMixed(const std::uint8_t addr[16])
{
    // first 12 bytes
    auto ret = ip_version6::compress(addr, addr + 12);

    // dotted decimal IPv4 address
    auto beg = addr + 12;

    for (auto it = beg, end = addr + 16; it != end; ++it)
    {
        ret += (it != beg) ? '.' : ':';
        ret.append(num::str(*it));
    }

    return ret;
}

std::array<std::uint8_t, 16> chen::ip_version6::toBytes(const std::string &addr)
{
    return ip_version6::toBytes(addr, nullptr);
};

std::array<std::uint8_t, 16> chen::ip_version6::toBytes(const std::string &addr, std::uint8_t *cidr)
{
    return ip_version6::toBytes(addr, cidr, nullptr);
};

std::array<std::uint8_t, 16> chen::ip_version6::toBytes(const std::string &addr, std::uint8_t *cidr, std::uint32_t *scope)
{
    std::array<std::uint8_t, 16> ret{};

    auto ptr = ret.begin();  // current insert position
    auto idx = ret.end();    // compressed position

    auto beg = addr.begin();
    auto end = addr.end();
    auto cur = beg;
    auto len = 8;  // maximum loop count, default is 8, if has dotted format, then change to 10
    bool hex = true;

    for (int i = 0; (i < len) && (cur != end) && (*cur != '/') && (*cur != '%'); ++i)
    {
        // check "::"
        if ((cur != beg) && (*cur == ':') && (*(cur - 1) == ':'))
        {
            if (idx == ret.end())
            {
                ++cur;
                idx = ptr;
                continue;
            }
            else
            {
                throw std::runtime_error("ipv6: multiple '::' is not allowed");
            }
        }

        // collect digits, up to 4
        char num[5]{};  // set all chars to zero in C++11

        for (int j = 0; (j < 5) && (cur != end); ++j)
        {
            char ch = static_cast<char>(std::tolower(*cur));

            if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')))
            {
                ++cur;
                num[j] = ch;
            }
            else if (ch == ':')
            {
                if (!hex)
                    throw std::runtime_error("ipv6: require decimal integer in dotted format");

                ++cur;
                break;
            }
            else if (ch == '.')
            {
                ++cur;
                len = 10;     // e.g: ::192.168.1.1, each integer is 1 byte
                hex = false;  // current chars are decimal integer
                break;
            }
            else if ((ch != '/') && (ch != '%'))
            {
                // unrecognized char
                throw std::runtime_error("ipv6: addr format is wrong");
            }
        }

        // parse the number
        if (hex)
        {
            std::uint16_t val = static_cast<std::uint16_t>(::strtol(num, nullptr, 16));
            std::uint8_t *tmp = reinterpret_cast<std::uint8_t*>(&val);

            *ptr++ = *(tmp + 1);
            *ptr++ = *tmp;
        }
        else
        {
            *ptr++ = static_cast<std::uint8_t>(std::atoi(num));
        }
    }

    // expand zeros
    if (idx != ret.end())
    {
        auto copy = ret.rbegin();

        for (auto it = ptr - 1; it >= idx; *it-- = 0)
            *copy++ = *it;
    }

    // scope id
    if ((cur != end) && (*cur == '%'))
    {
        std::string tmp;

        while ((++cur != end) && (*cur != '/'))
            tmp += *cur;

        if (scope)
            *scope = inet_adapter::scope(ret.data(), tmp);
    }
    else if (scope)
    {
        *scope = 0;
    }

    // CIDR notation
    if (cidr)
    {
        *cidr = 128;

        if ((cur != end) && (*cur == '/'))
        {
            std::uint32_t tmp = 0;

            while ((++cur != end) && std::isdigit(*cur))
                tmp = tmp * 10 + (*cur - '0');

            if (tmp > 128)
                throw std::runtime_error("ipv6: CIDR prefix must less than 128");

            *cidr = static_cast<uint8_t>(tmp);
        }
    }

    return ret;
}

// compress
std::string chen::ip_version6::compress(const std::uint8_t *beg, const std::uint8_t *end)
{
    std::string ret;
    int zero = 0;

    for (auto cur = beg; cur != end; cur += 2)
    {
        const std::uint8_t &a = *cur;
        const std::uint8_t &b = *(cur + 1);

        if (a || b || (zero < 0))
        {
            // last zero count
            if (zero == 1)
            {
                // append "0:" to string
                ret.append("0:");
                zero = 0;
            }
            else if (zero > 1)
            {
                // append ':' to string only once
                ret += ':';
                zero = -1;  // set to -1, don't allow two or more "::"
            }

            // append colon
            if (cur != beg)
                ret += ':';

            // append hexadecimal
            ret.append(str::format("%x", (static_cast<unsigned>(a) << 8) + b));
        }
        else
        {
            // two bytes are zero
            ++zero;
        }
    }

    // append trailing "::"
    if (zero > 0)
        ret.append("::");

    return ret;
}

std::uint8_t chen::ip_version6::toCIDR(const std::string &mask)
{
    return ip_version6::toCIDR(ip_version6::toBytes(mask).data());
}

std::uint8_t chen::ip_version6::toCIDR(const std::uint8_t mask[16])
{
    std::uint8_t cidr = 0;

    for (int i = 0; i < 16; ++i)
        cidr += static_cast<std::uint8_t>(num::bits(mask[i]));

    return cidr;
}