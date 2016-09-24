/**
 * Created by Jian Chen
 * @since  2016.08.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_interface.hpp>
#include <chen/base/num.hpp>
#include <chen/base/str.hpp>
#include <algorithm>
#include <cstring>
#include <cctype>

// -----------------------------------------------------------------------------
// version
std::uint8_t chen::ip::version::cidr() const
{
    // @see rfc1519
    return this->_cidr;
}

void chen::ip::version::cidr(std::uint8_t value)
{
    // @see rfc1519
    this->_cidr = value;
}


// -----------------------------------------------------------------------------
// version4
chen::ip::version4::version4(const char *addr)
{
    this->assign(addr);
}

chen::ip::version4::version4(const std::string &addr)
{
    this->assign(addr);
}

chen::ip::version4::version4(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip::version4::version4(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip::version4::version4(std::uint32_t addr)
{
    this->assign(addr);
}

chen::ip::version4::version4(std::uint32_t addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip::version4::version4(std::uint32_t addr, const std::string &mask)
{
    this->assign(addr, mask);
}

// assignment
void chen::ip::version4::assign()
{
    this->_addr = 0;
    this->_cidr = 32;
}

void chen::ip::version4::assign(const std::string &addr)
{
    this->_addr = version4::toInteger(addr, &this->_cidr);
}

void chen::ip::version4::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = version4::toInteger(addr);
    this->_cidr = cidr;

    if (this->_cidr > 32)
        throw std::runtime_error("ipv4: CIDR prefix must less than 32");
}

void chen::ip::version4::assign(const std::string &addr, const std::string &mask)
{
    this->_addr = version4::toInteger(addr);
    this->_cidr = version4::toCIDR(mask);
}

void chen::ip::version4::assign(std::uint32_t addr)
{
    this->_addr = addr;
    this->_cidr = 32;
}

void chen::ip::version4::assign(std::uint32_t addr, std::uint8_t cidr)
{
    this->_addr = addr;
    this->_cidr = cidr;

    if (this->_cidr > 32)
        throw std::runtime_error("ipv4: CIDR prefix must less than 32");
}

void chen::ip::version4::assign(std::uint32_t addr, const std::string &mask)
{
    this->_addr = addr;
    this->_cidr = version4::toCIDR(mask);
}

chen::ip::version4& chen::ip::version4::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

chen::ip::version4& chen::ip::version4::operator=(std::uint32_t addr)
{
    this->assign(addr);
    return *this;
}

// representation
std::string chen::ip::version4::str(bool cidr) const
{
    return !cidr ? version4::toString(this->_addr) : version4::toString(this->_addr, this->_cidr);
}

std::vector<std::uint8_t> chen::ip::version4::bytes() const
{
    return std::vector<std::uint8_t>{
            static_cast<std::uint8_t>(this->_addr >> 24 & 0xFF),
            static_cast<std::uint8_t>(this->_addr >> 16 & 0xFF),
            static_cast<std::uint8_t>(this->_addr >> 8 & 0xFF),
            static_cast<std::uint8_t>(this->_addr & 0xFF)
    };
}

std::uint32_t chen::ip::version4::addr() const
{
    // @see rfc791
    return this->_addr;
}

void chen::ip::version4::addr(std::uint32_t value)
{
    // @see rfc791
    this->_addr = value;
}

// network
std::uint32_t chen::ip::version4::netmask() const
{
    // @see rfc1878
    return 0xFFFFFFFFu << (32 - this->_cidr);
}

std::uint32_t chen::ip::version4::wildcard() const
{
    // @link https://en.wikipedia.org/wiki/Wildcard_mask
    return ~this->netmask();
}

chen::ip::version4 chen::ip::version4::network() const
{
    return version4(this->_addr & this->netmask(), this->_cidr);
}

chen::ip::version4 chen::ip::version4::minhost() const
{
    return version4((this->_addr & this->netmask()) | 0x00000001, this->_cidr);
}

chen::ip::version4 chen::ip::version4::maxhost() const
{
    return version4((this->_addr | this->wildcard()) & 0xFFFFFFFE, this->_cidr);
}

chen::ip::version4 chen::ip::version4::broadcast() const
{
    return version4(this->_addr | this->wildcard(), this->_cidr);
}

std::size_t chen::ip::version4::hosts() const
{
    return this->maxhost().addr() - this->minhost().addr() + 1;
}

// special
bool chen::ip::version4::isReserved() const
{
    // @link https://en.wikipedia.org/wiki/Reserved_IP_addresses

    // 0.0.0.0/8
    // @see rfc1700, page 4
    if ((this->_addr & 0xFF000000) == 0)
        return true;

    // loopback
    if (this->isLoopback())
        return true;

    // link-local
    if (this->isLinkLocal())
        return true;

    // 192.0.0.0/24, it's reserved, not private network according to its rfc
    // @see rfc5736, section 2
    if ((this->_addr & 0xFFFFFF00) == 0xC0000000)
        return true;

    // 192.0.2.0/24
    // @see rfc5737, section 3
    if ((this->_addr & 0xFFFFFF00) == 0xC0000200)
        return true;

    // 192.88.99.0/24
    // @see rfc3068, section 2.3
    if ((this->_addr & 0xFFFFFF00) == 0xC0586300)
        return true;

    // 198.51.100.0/24
    // @see rfc5737, section 3
    if ((this->_addr & 0xFFFFFF00) == 0xC6336400)
        return true;

    // 203.0.113.0/24
    // @see rfc5737, section 3
    if ((this->_addr & 0xFFFFFF00) == 0xCB007100)
        return true;

    // class D & E
    if (this->isClassD() || this->isClassE())
        return true;

    // private address is also a reserved address
    return this->isPrivate();
}

bool chen::ip::version4::isPrivate() const
{
    // 10.0.0.0/8
    // @see rfc1918, section 3
    if ((this->_addr & 0xFF000000) == 0x0A000000)
        return true;

    // 100.64.0.0/10
    // @see rfc6598, section 7
    if ((this->_addr & 0xFFC00000) == 0x64400000)
        return true;

    // 172.16.0.0/12
    // @see rfc1918, section 3
    if ((this->_addr & 0xFFF00000) == 0xAC100000)
        return true;

    // 192.168.0.0/16
    // @see rfc1918, section 3
    if ((this->_addr & 0xFFFF0000) == 0xC0A80000)
        return true;

    // 198.18.0.0/15
    // @see rfc2544
    return (this->_addr & 0xFFFE0000) == 0xC6120000;
}

bool chen::ip::version4::isLoopback() const
{
    // 127.0.0.0/8
    // @see rfc990 & rfc6890, section 2.2.2
    return (this->_addr & 0xFF000000) == 0x7F000000;
}

bool chen::ip::version4::isLinkLocal() const
{
    // 169.254.0.0/16
    // @see rfc3927
    return (this->_addr & 0xFFFF0000) == 0xA9FE0000;
}

bool chen::ip::version4::isMulticast() const
{
    return this->isClassD();
}

bool chen::ip::version4::isBroadcast() const
{
    // host bits are 1
    return (this->_addr | this->wildcard()) == this->_addr;
}

// classful
bool chen::ip::version4::isClassA() const
{
    // leading: 0, network: 8, range: 0.0.0.0 ~ 127.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0x80000000) == 0;
}

bool chen::ip::version4::isClassB() const
{
    // leading: 10, network: 16, range: 128.0.0.0 ~ 191.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0xC0000000) == 0x80000000;
}

bool chen::ip::version4::isClassC() const
{
    // leading: 110, network: 24, range: 192.0.0.0 ~ 223.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0xE0000000) == 0xC0000000;
}

bool chen::ip::version4::isClassD() const
{
    // leading: 1110, range: 224.0.0.0 ~ 239.255.255.255
    // @see rfc1112, section 4
    return (this->_addr & 0xF0000000) == 0xE0000000;
}

bool chen::ip::version4::isClassE() const
{
    // leading: 1111, range: 240.0.0.0 ~ 255.255.255.255
    // @see rfc1112, section 4
    return (this->_addr & 0xF0000000) == 0xF0000000;
}

// operator
bool chen::ip::version4::operator==(const version4 &o) const
{
    return (this->_addr == o._addr) && (this->_cidr == o._cidr);
}

bool chen::ip::version4::operator!=(const version4 &o) const
{
    return !(*this == o);
}

bool chen::ip::version4::operator<(const version4 &o) const
{
    return (this->_addr == o._addr) ? this->_cidr < o._cidr : this->_addr < o._addr;
}

bool chen::ip::version4::operator>(const version4 &o) const
{
    return o < *this;
}

bool chen::ip::version4::operator<=(const version4 &o) const
{
    return (this->_addr == o._addr) ? this->_cidr <= o._cidr : this->_addr <= o._addr;
}

bool chen::ip::version4::operator>=(const version4 &o) const
{
    return o <= *this;
}

// convert
std::string chen::ip::version4::toString(std::uint32_t addr)
{
    return str::format("%u.%u.%u.%u",
                       addr >> 24 & 0xFF,
                       addr >> 16 & 0xFF,
                       addr >> 8 & 0xFF,
                       addr & 0xFF);
}

std::string chen::ip::version4::toString(std::uint32_t addr, std::uint8_t cidr)
{
    return version4::toString(addr) + "/" + num::str(cidr);
}

std::uint32_t chen::ip::version4::toInteger(const std::string &addr)
{
    return version4::toInteger(addr, nullptr);
}

std::uint32_t chen::ip::version4::toInteger(const std::string &addr, std::uint8_t *cidr)
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
        } while (std::isdigit(*cur));

        if (*cur == '.')
            ++cur;

        // check if valid
        if (num[i] > 0xFF)
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

        if (*cur == '/')
        {
            std::uint32_t tmp = 0;

            while ((cur != end) && std::isdigit(*++cur))
                tmp = tmp * 10 + (*cur - '0');

            if (tmp > 32)
                throw std::runtime_error("ipv4: CIDR prefix must less than 32");

            *cidr = static_cast<uint8_t>(tmp);
        }
    }

    return val;
}

std::uint8_t chen::ip::version4::toCIDR(const std::string &mask)
{
    return version4::toCIDR(version4::toInteger(mask));
}

std::uint8_t chen::ip::version4::toCIDR(std::uint32_t mask)
{
    return static_cast<std::uint8_t>(num::bits(mask));
}


// -----------------------------------------------------------------------------
// version6
chen::ip::version6::version6(const char *addr)
{
    this->assign(addr);
}

chen::ip::version6::version6(const std::string &addr)
{
    this->assign(addr);
}

chen::ip::version6::version6(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip::version6::version6(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::ip::version6::version6(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip::version6::version6(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

chen::ip::version6::version6(const std::uint8_t addr[16])
{
    this->assign(addr);
}

chen::ip::version6::version6(const std::uint8_t addr[16], std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

chen::ip::version6::version6(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope)
{
    this->assign(addr, cidr, scope);
}

chen::ip::version6::version6(const std::uint8_t addr[16], const std::string &mask)
{
    this->assign(addr, mask);
}

chen::ip::version6::version6(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope)
{
    this->assign(addr, mask, scope);
}

// assignment
void chen::ip::version6::assign()
{
    this->_addr.fill(0);
    this->_cidr  = 128;
    this->_scope = 0;
}

void chen::ip::version6::assign(const std::string &addr)
{
    this->_addr = version6::toBytes(addr, &this->_cidr, &this->_scope);
}

void chen::ip::version6::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = version6::toBytes(addr, nullptr, &this->_scope);
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw std::runtime_error("ipv6: CIDR prefix must less than 128");
}

void chen::ip::version6::assign(const std::string &addr, std::uint8_t cidr, std::uint32_t scope)
{
    this->_addr  = version6::toBytes(addr);
    this->_cidr  = cidr;
    this->_scope = scope;

    if (this->_cidr > 128)
        throw std::runtime_error("ipv6: CIDR prefix must less than 128");
}

void chen::ip::version6::assign(const std::string &addr, const std::string &mask)
{
    this->_addr = version6::toBytes(addr, nullptr, &this->_scope);
    this->_cidr = version6::toCIDR(mask);
}

void chen::ip::version6::assign(const std::string &addr, const std::string &mask, std::uint32_t scope)
{
    this->_addr  = version6::toBytes(addr);
    this->_cidr  = version6::toCIDR(mask);
    this->_scope = scope;
}

void chen::ip::version6::assign(const std::uint8_t addr[16])
{
    this->assign(addr, 128, 0);
}

void chen::ip::version6::assign(const std::uint8_t addr[16], std::uint8_t cidr)
{
    this->assign(addr, cidr, 0);
}

void chen::ip::version6::assign(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope)
{
    ::memcpy(this->_addr.data(), addr, 16);

    this->_cidr  = cidr;
    this->_scope = scope;

    if (this->_cidr > 128)
        throw std::runtime_error("ipv6: CIDR prefix must less than 128");
}

void chen::ip::version6::assign(const std::uint8_t addr[16], const std::string &mask)
{
    this->assign(addr, mask, 0);
}

void chen::ip::version6::assign(const std::uint8_t addr[16], const std::string &mask, std::uint32_t scope)
{
    ::memcpy(this->_addr.data(), addr, 16);

    this->_cidr  = version6::toCIDR(mask);
    this->_scope = scope;
}

chen::ip::version6& chen::ip::version6::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

chen::ip::version6& chen::ip::version6::operator=(const std::uint8_t addr[16])
{
    this->assign(addr);
    return *this;
}

// representation
std::string chen::ip::version6::str(bool cidr) const
{
    return this->str(cidr, false);
}

std::string chen::ip::version6::str(bool cidr, bool scope) const
{
    if (cidr && scope)
        return version6::toScope(this->_addr.data(), this->_cidr, this->_scope);
    else if (cidr)
        return version6::toString(this->_addr.data(), this->_cidr);
    else if (scope)
        return version6::toScope(this->_addr.data(), this->_scope);
    else
        return version6::toString(this->_addr.data());
}

std::vector<std::uint8_t> chen::ip::version6::bytes() const
{
    return std::vector<std::uint8_t>(this->_addr.begin(), this->_addr.end());
}

std::string chen::ip::version6::expanded() const
{
    return version6::toExpanded(this->_addr.data());
}

std::string chen::ip::version6::suppressed() const
{
    return version6::toSuppressed(this->_addr.data());
}

std::string chen::ip::version6::compressed() const
{
    return version6::toCompressed(this->_addr.data());
}

std::string chen::ip::version6::mixed() const
{
    return version6::toMixed(this->_addr.data());
}

chen::ip::version4 chen::ip::version6::embedded() const
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

    return version4(a * 256u * 256u * 256u +
                    b * 256u * 256u +
                    c * 256u +
                    d);
}

const std::array<std::uint8_t, 16>& chen::ip::version6::addr() const
{
    // @see rfc4291
    return this->_addr;
}

void chen::ip::version6::addr(const std::uint8_t value[16])
{
    // @see rfc4291
    ::memcpy(this->_addr.data(), value, 16);
}

std::uint32_t chen::ip::version6::scope() const
{
    // @see rfc4007
    return this->_scope;
}

void chen::ip::version6::scope(std::uint32_t value)
{
    // @see rfc4007
    this->_scope = value;
}

// network
std::array<std::uint8_t, 16> chen::ip::version6::netmask() const
{
    std::array<std::uint8_t, 16> ret{};

    auto len = this->_cidr / 8;
    auto mod = this->_cidr % 8;

    auto i = 0;

    for (; i < len; ++i)
        ret[i] = 0xFF;

    if (mod)
        ret[i] = static_cast<std::uint8_t>(0xFF << (8 - mod));

    return ret;
};

std::array<std::uint8_t, 16> chen::ip::version6::wildcard() const
{
    std::array<std::uint8_t, 16> ret{};

    auto len = this->_cidr / 8;
    auto mod = this->_cidr % 8;

    auto it = ret.begin() + len;

    if (mod)
    {
        ++it;
        ret[len] = static_cast<std::uint8_t>(~(0xFF << (8 - mod)));
    }

    for (; it != ret.end(); ++it)
        *it = 0xFF;

    return ret;
};

chen::ip::version6 chen::ip::version6::network() const
{
    std::uint8_t tmp[16]{};
    std::array<std::uint8_t, 16> mask = this->netmask();

    for (std::size_t i = 0, len = mask.size(); i < len; ++i)
        tmp[i] = this->_addr[i] & mask[i];

    return version6(tmp, this->_cidr, this->_scope);
}

chen::ip::version6 chen::ip::version6::minhost() const
{
    // IPv6 host begins with 0
    return this->network();
}

chen::ip::version6 chen::ip::version6::maxhost() const
{
    // IPv6 host ends with 1
    std::uint8_t tmp[16]{};
    std::array<std::uint8_t, 16> mask = this->wildcard();

    for (std::size_t i = 0, len = mask.size(); i < len; ++i)
        tmp[i] = this->_addr[i] | mask[i];

    return version6(tmp, this->_cidr, this->_scope);
}

// special
bool chen::ip::version6::isUnspecified() const
{
    // all bits are zero
    // @see rfc4291, section 2.5.2
    return std::all_of(this->_addr.begin(), this->_addr.end(), [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

bool chen::ip::version6::isLoopback() const
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

bool chen::ip::version6::isGlobalUnicast() const
{
    // first 3 bits are 001
    // @see rfc3587, section 3
    return (this->_addr[0] & 0xE0) == 0x20;
}

bool chen::ip::version6::isLinkLocalUnicast() const
{
    // first 10 bits are 1111111010
    // @see rfc4291, section 2.5.6
    return ((this->_addr[0] == 0xFE) && ((this->_addr[1] & 0xC0) == 0x80));
}

bool chen::ip::version6::isSiteLocalUnicast() const
{
    // first 10 bits are 1111111011
    // @see rfc4291, section 2.5.7
    return ((this->_addr[0] == 0xFE) && ((this->_addr[1] & 0xC0) == 0xC0));
}

bool chen::ip::version6::isIPv4Compatible() const
{
    // first 96 bits are zero
    // @see rfc4291, section 2.5.5.1
    return std::all_of(this->_addr.begin(), this->_addr.begin() + 12, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

bool chen::ip::version6::isIPv4Mapped() const
{
    // first 80 bits are zero, next 16 bits are one
    // @see rfc4291, section 2.5.5.2
    auto ret = std::all_of(this->_addr.begin(), this->_addr.begin() + 10, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });

    if (!ret)
        return false;

    return (this->_addr[10] == 0xFF) && (this->_addr[11] == 0xFF);
}

bool chen::ip::version6::isMulticast() const
{
    // first 8 bits is 0xFF
    // @see rfc4291, section 2.7
    return this->_addr[0] == 0xFF;
}

// NAT64
bool chen::ip::version6::isIPv4EmbeddedWellKnown() const
{
    // "64:ff9b::/96"
    // @see rfc6052, section 2.1
    if ((this->_addr[0] != 0) || (this->_addr[1] != 0x64) || (this->_addr[2] != 0xFF) || (this->_addr[3] != 0x9b))
        return false;

    return std::all_of(this->_addr.begin() + 4, this->_addr.begin() + 12, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

// operator
bool chen::ip::version6::operator==(const version6 &o) const
{
    return (this->_addr == o._addr) && (this->_cidr == o._cidr) && (this->_scope == o._scope);
}

bool chen::ip::version6::operator!=(const version6 &o) const
{
    return !(*this == o);
}

bool chen::ip::version6::operator<(const version6 &o) const
{
    if (this->_addr == o._addr)
        return this->_cidr == o._cidr ? this->_scope < o._scope : this->_cidr < o._cidr;
    else
        return this->_addr < o._addr;
}

bool chen::ip::version6::operator>(const version6 &o) const
{
    return o < *this;
}

bool chen::ip::version6::operator<=(const version6 &o) const
{
    if (this->_addr == o._addr)
        return this->_cidr == o._cidr ? this->_scope <= o._scope : this->_cidr <= o._cidr;
    else
        return this->_addr <= o._addr;
}

bool chen::ip::version6::operator>=(const version6 &o) const
{
    return o <= *this;
}

// convert
std::string chen::ip::version6::toString(const std::uint8_t addr[16])
{
    return version6::toCompressed(addr);
}

std::string chen::ip::version6::toString(const std::uint8_t addr[16], std::uint8_t cidr)
{
    return version6::toCompressed(addr) + "/" + num::str(cidr);
}

std::string chen::ip::version6::toScope(const std::uint8_t addr[16], std::uint32_t scope)
{
    return version6::toCompressed(addr) + "%" + interface::scope(scope);
}

std::string chen::ip::version6::toScope(const std::uint8_t addr[16], std::uint8_t cidr, std::uint32_t scope)
{
    return version6::toCompressed(addr) + "%" + interface::scope(scope) + "/" + num::str(cidr);
}

std::string chen::ip::version6::toExpanded(const std::uint8_t addr[16])
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

std::string chen::ip::version6::toSuppressed(const std::uint8_t addr[16])
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

std::string chen::ip::version6::toCompressed(const std::uint8_t addr[16])
{
    return version6::compress(addr, addr + 16);
}

std::string chen::ip::version6::toMixed(const std::uint8_t addr[16])
{
    // first 12 bytes
    auto ret = version6::compress(addr, addr + 12);

    // dotted decimal IPv4 address
    auto beg = addr + 12;

    for (auto it = beg, end = addr + 16; it != end; ++it)
    {
        ret += (it != beg) ? '.' : ':';
        ret.append(num::str(*it));
    }

    return ret;
}

std::array<std::uint8_t, 16> chen::ip::version6::toBytes(const std::string &addr)
{
    return version6::toBytes(addr, nullptr);
};

std::array<std::uint8_t, 16> chen::ip::version6::toBytes(const std::string &addr, std::uint8_t *cidr)
{
    return version6::toBytes(addr, cidr, nullptr);
};

std::array<std::uint8_t, 16> chen::ip::version6::toBytes(const std::string &addr, std::uint8_t *cidr, std::uint32_t *scope)
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
    if (*cur == '%')
    {
        std::string tmp;

        while ((cur != end) && (*++cur != '/') && *cur)
            tmp += *cur;

        if (scope)
            *scope = interface::scope(ret.data(), tmp);
    }
    else if (scope)
    {
        *scope = 0;
    }

    // CIDR notation
    if (cidr)
    {
        *cidr = 128;

        if (*cur == '/')
        {
            std::uint32_t tmp = 0;

            while ((cur != end) && std::isdigit(*++cur))
                tmp = tmp * 10 + (*cur - '0');

            if (tmp > 128)
                throw std::runtime_error("ipv6: CIDR prefix must less than 128");

            *cidr = static_cast<uint8_t>(tmp);
        }
    }

    return ret;
}

// compress
std::string chen::ip::version6::compress(const std::uint8_t *beg, const std::uint8_t *end)
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

std::uint8_t chen::ip::version6::toCIDR(const std::string &mask)
{
    return version6::toCIDR(version6::toBytes(mask).data());
}

std::uint8_t chen::ip::version6::toCIDR(const std::uint8_t mask[16])
{
    std::uint8_t cidr = 0;

    for (int i = 0; i < 16; ++i)
        cidr += static_cast<std::uint8_t>(num::bits(mask[i]));

    return cidr;
}