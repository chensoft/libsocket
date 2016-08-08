/**
 * Created by Jian Chen
 * @since  2016.07.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_address.hpp>
#include <socket/ip/ip_subnet.hpp>
#include <socket/ip/ip_error.hpp>
#include <chen/base/num.hpp>
#include <chen/base/str.hpp>
#include <algorithm>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// address
const address_v4* address::v4() const
{
    return dynamic_cast<const address_v4*>(this);
}

const address_v6* address::v6() const
{
    return dynamic_cast<const address_v6*>(this);
}

address_v4* address::v4()
{
    return dynamic_cast<address_v4*>(this);
}

address_v6* address::v6()
{
    return dynamic_cast<address_v6*>(this);
}

// operator
bool address::operator!=(const address &o) const
{
    return !(*this == o);
}

bool address::operator>(const address &o) const
{
    return o < *this;
}

bool address::operator>=(const address &o) const
{
    return o <= *this;
}

// create
std::shared_ptr<chen::ip::address> address::create(const std::string &addr)
{
    if (address::isIPv4(addr))
        return std::make_shared<chen::ip::address_v4>(addr);
    else
        return std::make_shared<chen::ip::address_v6>(addr);
}

std::shared_ptr<chen::ip::address> address::create(const std::string &addr, std::uint8_t cidr)
{
    if (address::isIPv4(addr))
        return std::make_shared<chen::ip::subnet_v4>(addr, cidr);
    else
        return std::make_shared<chen::ip::address_v6>(addr, cidr);
}

bool address::isIPv4(const std::string &addr)
{
    return !str::contain(addr, ":");
}

bool address::isIPv6(const std::string &addr)
{
    return str::contain(addr, ":");
}


// -----------------------------------------------------------------------------
// address_v4
address_v4::address_v4(const std::string &addr)
{
    this->assign(addr);
}

address_v4::address_v4(std::uint32_t addr)
{
    this->assign(addr);
}

std::shared_ptr<chen::ip::address> address_v4::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

// assignment
void address_v4::assign(const std::string &addr)
{
    this->_addr = address_v4::toInteger(addr);
}

void address_v4::assign(std::uint32_t addr)
{
    this->_addr = addr;
}

address& address_v4::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

address& address_v4::operator=(std::uint32_t addr)
{
    this->assign(addr);
    return *this;
}

// representation
std::string address_v4::str() const
{
    return address_v4::toString(this->_addr);
}

std::vector<std::uint8_t> address_v4::bytes() const
{
    return std::vector<std::uint8_t>{
            static_cast<std::uint8_t>(this->_addr >> 24 & 0xFF),
            static_cast<std::uint8_t>(this->_addr >> 16 & 0xFF),
            static_cast<std::uint8_t>(this->_addr >> 8 & 0xFF),
            static_cast<std::uint8_t>(this->_addr & 0xFF)
    };
}

const std::uint32_t& address_v4::addr() const
{
    // @see rfc791
    return this->_addr;
}

std::uint32_t& address_v4::addr()
{
    return this->_addr;
}

// special
bool address_v4::isReserved() const
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

bool address_v4::isPrivate() const
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

bool address_v4::isLoopback() const
{
    // 127.0.0.0/8
    // @see rfc990 & rfc6890, section 2.2.2
    return (this->_addr & 0xFF000000) == 0x7F000000;
}

bool address_v4::isLinkLocal() const
{
    // 169.254.0.0/16
    // @see rfc3927
    return (this->_addr & 0xFFFF0000) == 0xA9FE0000;
}

bool address_v4::isMulticast() const
{
    return this->isClassD();
}

// classful
bool address_v4::isClassA() const
{
    // leading: 0, network: 8, range: 0.0.0.0 ~ 127.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0x80000000) == 0;
}

bool address_v4::isClassB() const
{
    // leading: 10, network: 16, range: 128.0.0.0 ~ 191.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0xC0000000) == 0x80000000;
}

bool address_v4::isClassC() const
{
    // leading: 110, network: 24, range: 192.0.0.0 ~ 223.255.255.255
    // @see rfc791, section 3.2
    return (this->_addr & 0xE0000000) == 0xC0000000;
}

bool address_v4::isClassD() const
{
    // leading: 1110, range: 224.0.0.0 ~ 239.255.255.255
    // @see rfc1112, section 4
    return (this->_addr & 0xF0000000) == 0xE0000000;
}

bool address_v4::isClassE() const
{
    // leading: 1111, range: 240.0.0.0 ~ 255.255.255.255
    // @see rfc1112, section 4
    return (this->_addr & 0xF0000000) == 0xF0000000;
}

// operator
bool address_v4::operator==(const address &o) const
{
    const address_v4 &a = dynamic_cast<const address_v4&>(o);
    return (this->_addr == a._addr);
}

bool address_v4::operator<(const address &o) const
{
    const address_v4 &a = dynamic_cast<const address_v4&>(o);
    return this->_addr < a._addr;
}

bool address_v4::operator<=(const address &o) const
{
    const address_v4 &a = dynamic_cast<const address_v4&>(o);
    return this->_addr <= a._addr;
}

// convert
std::string address_v4::toString(std::uint32_t addr)
{
    return str::format("%u.%u.%u.%u",
                       addr >> 24 & 0xFF,
                       addr >> 16 & 0xFF,
                       addr >> 8 & 0xFF,
                       addr & 0xFF);
}

std::string address_v4::toString(std::uint32_t addr, std::uint8_t cidr)
{
    return address_v4::toString(addr) + "/" + num::str(cidr);
}

std::uint32_t address_v4::toInteger(const std::string &addr)
{
    std::uint8_t cidr = 0;
    return address_v4::toInteger(addr, cidr);
}

std::uint32_t address_v4::toInteger(const std::string &addr, std::uint8_t &cidr)
{
    auto cur = addr.begin();
    auto end = addr.end();

    std::uint32_t idx   = 0;
    std::uint32_t num[] = {0, 0, 0, 0};

    for (int i = 0; (i < 4) && (cur != end) && (*cur != '/'); ++i, ++idx)
    {
        // check character
        if (!std::isdigit(*cur))
            throw error_address("ipv4: addr format is wrong");

        // collect digits
        do
        {
            num[i] = num[i] * 10 + (*cur++ - '0');
        } while (std::isdigit(*cur));

        if (*cur == '.')
            ++cur;

        // check if valid
        if (num[i] > 0xFF)
            throw error_address("ipv4: addr number must between 0 and 255");
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
    cidr = 32;

    if (*cur == '/')
    {
        std::uint32_t tmp = 0;

        while (std::isdigit(*++cur))
            tmp = tmp * 10 + (*cur - '0');

        if (tmp > 32)
            throw error_subnet("ipv4: CIDR prefix must less than 32");

        cidr = static_cast<uint8_t>(tmp);
    }

    return val;
}

// common
address_v4 address_v4::any()
{
    return address_v4();
}

address_v4 address_v4::broad()
{
    return address_v4(0xFFFFFFFFu);
}

address_v4 address_v4::loopback()
{
    return address_v4(0x7F000001);
}


// -----------------------------------------------------------------------------
// address_v6
address_v6::address_v6() : _cidr(128)
{
}

address_v6::address_v6(const std::string &addr)
{
    this->assign(addr);
}

address_v6::address_v6(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

address_v6::address_v6(const std::array<std::uint8_t, 16> &addr)
{
    this->assign(addr);
}

address_v6::address_v6(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

address_v6::address_v6(std::array<std::uint8_t, 16> &&addr)
{
    this->assign(std::move(addr));
}

address_v6::address_v6(std::array<std::uint8_t, 16> &&addr, std::uint8_t cidr)
{
    this->assign(std::move(addr), cidr);
}

std::shared_ptr<chen::ip::address> address_v6::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

// assignment
void address_v6::assign(const std::string &addr)
{
    this->_addr = address_v6::toBytes(addr, this->_cidr);
}

void address_v6::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = std::move(address_v6::toBytes(addr));
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw error_address("ipv6: CIDR prefix must less than 128");
}

void address_v6::assign(const std::array<std::uint8_t, 16> &addr)
{
    this->assign(addr, 128);
}

void address_v6::assign(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr)
{
    this->_addr = addr;
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw error_address("ipv6: CIDR prefix must less than 128");
}

void address_v6::assign(std::array<std::uint8_t, 16> &&addr)
{
    this->assign(std::move(addr), 128);
}

void address_v6::assign(std::array<std::uint8_t, 16> &&addr, std::uint8_t cidr)
{
    this->_addr = std::move(addr);
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw error_address("ipv6: CIDR prefix must less than 128");
}

address& address_v6::operator=(const std::string &addr)
{
    this->assign(addr);
    return *this;
}

// representation
std::string address_v6::str() const
{
    return address_v6::toString(this->_addr);
}

std::vector<std::uint8_t> address_v6::bytes() const
{
    return std::vector<std::uint8_t>(this->_addr.begin(), this->_addr.end());
}

std::string address_v6::notation() const
{
    return address_v6::toString(this->_addr, this->_cidr);
}

std::string address_v6::expanded() const
{
    return address_v6::toExpanded(this->_addr);
}

std::string address_v6::suppressed() const
{
    return address_v6::toSuppressed(this->_addr);
}

std::string address_v6::compressed() const
{
    return address_v6::toCompressed(this->_addr);
}

std::string address_v6::mixed() const
{
    return address_v6::toMixed(this->_addr);
}

address_v4 address_v6::embedded() const
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

    return address_v4(a * 256u * 256u * 256u +
                      b * 256u * 256u +
                      c * 256u +
                      d);
}

const std::array<std::uint8_t, 16>& address_v6::addr() const
{
    return this->_addr;
}

const std::uint8_t& address_v6::cidr() const
{
    return this->_cidr;
}

std::array<std::uint8_t, 16>& address_v6::addr()
{
    return this->_addr;
}

std::uint8_t& address_v6::cidr()
{
    return this->_cidr;
}

std::array<std::uint8_t, 16> address_v6::netmask() const
{
    std::array<std::uint8_t, 16> ret{};

    std::uint8_t len = static_cast<std::uint8_t>(this->_cidr / 8);
    std::uint8_t mod = static_cast<std::uint8_t>(this->_cidr % 8);

    std::uint8_t i = 0;

    for (; i < len; ++i)
        ret[i] = 0xFF;

    if (mod)
        ret[i] = static_cast<std::uint8_t>(0xFF << (8 - mod));

    return ret;
};

std::array<std::uint8_t, 16> address_v6::wildcard() const
{
    std::array<std::uint8_t, 16> ret{};

    std::uint8_t len = static_cast<std::uint8_t>(this->_cidr / 8);
    std::uint8_t mod = static_cast<std::uint8_t>(this->_cidr % 8);

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

// network
address_v6 address_v6::network() const
{
    std::array<std::uint8_t, 16> ret{};
    std::array<std::uint8_t, 16> mask = this->netmask();

    for (std::size_t i = 0, len = ret.size(); i < len; ++i)
        ret[i] = this->_addr[i] & mask[i];

    return address_v6(ret, this->_cidr);
}

address_v6 address_v6::minhost() const
{
    // IPv6 host begins with 0
    return this->network();
}

address_v6 address_v6::maxhost() const
{
    // IPv6 host ends with 1
    std::array<std::uint8_t, 16> ret{};
    std::array<std::uint8_t, 16> mask = this->wildcard();

    for (std::size_t i = 0, len = ret.size(); i < len; ++i)
        ret[i] = this->_addr[i] | mask[i];

    return address_v6(ret, this->_cidr);
}

// special
bool address_v6::isUnspecified() const
{
    // all bits are zero
    // @see rfc4291, section 2.5.2
    return std::all_of(this->_addr.begin(), this->_addr.end(), [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

bool address_v6::isLoopback() const
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

bool address_v6::isGlobalUnicast() const
{
    // first 3 bits are 001
    // @see rfc3587, section 3
    return (this->_addr[0] & 0xE0) == 0x20;
}

bool address_v6::isLinkLocalUnicast() const
{
    // first 10 bits are 1111111010
    // @see rfc4291, section 2.5.6
    return ((this->_addr[0] == 0xFE) && ((this->_addr[1] & 0xC0) == 0x80));
}

bool address_v6::isSiteLocalUnicast() const
{
    // first 10 bits are 1111111011
    // @see rfc4291, section 2.5.7
    return ((this->_addr[0] == 0xFE) && ((this->_addr[1] & 0xC0) == 0xC0));
}

bool address_v6::isIPv4Compatible() const
{
    // first 96 bits are zero
    // @see rfc4291, section 2.5.5.1
    return std::all_of(this->_addr.begin(), this->_addr.begin() + 12, [] (const std::uint8_t &ch) -> bool {
        return ch == 0;
    });
}

bool address_v6::isIPv4Mapped() const
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

bool address_v6::isMulticast() const
{
    // first 8 bits is 0xFF
    // @see rfc4291, section 2.7
    return this->_addr[0] == 0xFF;
}

// NAT64
bool address_v6::isIPv4EmbeddedWellKnown() const
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
bool address_v6::operator==(const address &o) const
{
    const address_v6 &a = dynamic_cast<const address_v6&>(o);
    return (this->_addr == a._addr) && (this->_cidr == a._cidr);
}

bool address_v6::operator<(const address &o) const
{
    const address_v6 &a = dynamic_cast<const address_v6&>(o);
    return (this->_addr == a._addr) ? this->_cidr < a._cidr : this->_addr < a._addr;
}

bool address_v6::operator<=(const address &o) const
{
    const address_v6 &a = dynamic_cast<const address_v6&>(o);
    return (this->_addr == a._addr) ? this->_cidr <= a._cidr : this->_addr < a._addr;
}

// convert
std::string address_v6::toString(const std::array<std::uint8_t, 16> &addr)
{
    return address_v6::toCompressed(addr);
}

std::string address_v6::toString(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr)
{
    return address_v6::toCompressed(addr) + "/" + num::str(cidr);
}

std::string address_v6::toExpanded(const std::array<std::uint8_t, 16> &addr)
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

std::string address_v6::toSuppressed(const std::array<std::uint8_t, 16> &addr)
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

std::string address_v6::toCompressed(const std::array<std::uint8_t, 16> &addr)
{
    return address_v6::compress(addr.begin(), addr.end());
}

std::string address_v6::toMixed(const std::array<std::uint8_t, 16> &addr)
{
    // first 12 bytes
    auto ret = address_v6::compress(addr.begin(), addr.begin() + 12);

    // dotted decimal IPv4 address
    auto beg = addr.begin() + 12;

    for (auto it = beg; it != addr.end(); ++it)
    {
        ret += (it != beg) ? '.' : ':';
        ret.append(num::str(*it));
    }

    return ret;
}

std::array<std::uint8_t, 16> address_v6::toBytes(const std::string &addr)
{
    std::uint8_t cidr = 0;
    return address_v6::toBytes(addr, cidr);
};

std::array<std::uint8_t, 16> address_v6::toBytes(const std::string &addr, std::uint8_t &cidr)
{
    std::array<std::uint8_t, 16> ret{};

    auto ptr = ret.begin();  // current insert position
    auto idx = ret.end();    // compressed position

    auto beg = addr.begin();
    auto end = addr.end();
    auto cur = beg;
    auto len = 8;  // maximum loop count, default is 8, if has dotted format, then change to 10
    bool hex = true;

    for (int i = 0; (i < len) && (cur != end) && (*cur != '/'); ++i)
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
                throw error_address("ipv6: multiple '::' is not allowed");
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
                    throw error_address("ipv6: require decimal integer in dotted format");

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
            else if (ch != '/')
            {
                // unrecognized char
                throw error_address("ipv6: addr format is wrong");
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

    // CIDR notation
    cidr = 128;

    if (*cur == '/')
    {
        std::uint32_t tmp = 0;

        while (std::isdigit(*++cur))
            tmp = tmp * 10 + (*cur - '0');

        if (tmp > 128)
            throw error_address("ipv6: CIDR prefix must less than 128");

        cidr = static_cast<uint8_t>(tmp);
    }

    return ret;
};

// common
address_v6 address_v6::any()
{
    return address_v6();
}

address_v6 address_v6::loopback()
{
    return address_v6(std::array<std::uint8_t, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01});
}

// compress
std::string address_v6::compress(std::array<std::uint8_t, 16>::const_iterator beg,
                                 std::array<std::uint8_t, 16>::const_iterator end)
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