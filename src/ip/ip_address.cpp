/**
 * Created by Jian Chen
 * @since  2016.07.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_address.hpp>
#include <socket/ip/ip_error.hpp>
#include <chen/base/num.hpp>
#include <chen/base/str.hpp>
#include <bitset>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// address
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


// -----------------------------------------------------------------------------
// address_v4
address_v4::address_v4(const std::string &addr)
{
    this->_addr = address_v4::toInteger(addr, this->_cidr);
}

address_v4::address_v4(const std::string &addr, std::uint8_t cidr)
: _addr(address_v4::toInteger(addr))
, _cidr(cidr)
{
}

address_v4::address_v4(const std::string &addr, const std::string &mask)
: _addr(address_v4::toInteger(addr))
, _cidr(static_cast<std::uint8_t>(std::bitset<32>(address_v4::toInteger(mask)).count()))
{
}

address_v4::address_v4(std::uint32_t addr)
: _addr(addr)
, _cidr(32)
{
}

address_v4::address_v4(std::uint32_t addr, std::uint8_t cidr)
: _addr(addr)
, _cidr(cidr)
{
}

address_v4::address_v4(std::uint32_t addr, const std::string &mask)
: _addr(addr)
, _cidr(static_cast<std::uint8_t>(std::bitset<32>(address_v4::toInteger(mask)).count()))
{
}

// representation
std::string address_v4::str() const
{
    return address_v4::toString(this->_addr);
}

std::string address_v4::full() const
{
    return address_v4::toString(this->_addr, this->_cidr);
}

std::vector<std::uint8_t> address_v4::bytes() const
{
    const std::uint8_t *ptr = reinterpret_cast<const std::uint8_t*>(&this->_addr);

    std::vector<std::uint8_t> ret(4);
    std::copy(ptr, ptr + 4, ret.rbegin());

    return ret;
}

std::uint32_t address_v4::addr() const
{
    // @see rfc791
    return this->_addr;
}

std::uint8_t address_v4::cidr() const
{
    // @see rfc1519
    return this->_cidr;
}

std::uint32_t address_v4::netmask() const
{
    // @see rfc1878
    return 0xFFFFFFFFu << (32 - this->_cidr);
}

std::uint32_t address_v4::wildcard() const
{
    // @link https://en.wikipedia.org/wiki/Wildcard_mask
    return ~this->netmask();
}

// network
address_v4 address_v4::network() const
{
    return address_v4(this->_addr & this->netmask(), this->_cidr);
}

address_v4 address_v4::minhost() const
{
    return address_v4((this->_addr & this->netmask()) | 0x00000001, this->_cidr);
}

address_v4 address_v4::maxhost() const
{
    return address_v4((this->_addr | this->wildcard()) & 0xFFFFFFFE, this->_cidr);
}

address_v4 address_v4::broadcast() const
{
    return address_v4(this->_addr | this->wildcard(), this->_cidr);
}

// hosts
std::size_t address_v4::hosts() const
{
    return this->maxhost().addr() - this->minhost().addr() + 1;
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

bool address_v4::isBroadcast() const
{
    // host bits are 1
    std::uint32_t broadcast = (this->_addr | this->wildcard());
    return broadcast == this->_addr;
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
    return this->_addr == dynamic_cast<const address_v4&>(o)._addr;
}

bool address_v4::operator<(const address &o) const
{
    return this->_addr < dynamic_cast<const address_v4&>(o)._addr;
}

bool address_v4::operator<=(const address &o) const
{
    return this->_addr <= dynamic_cast<const address_v4&>(o)._addr;
}

// convert
std::string address_v4::toString(std::uint32_t addr)
{
    return str::format("%u.%u.%u.%u",
                       (addr >> 24) & 0xFF,
                       (addr >> 16) & 0xFF,
                       (addr >> 8) & 0xFF,
                       addr & 0xFF);
}

std::string address_v4::toString(std::uint32_t addr, std::uint8_t cidr)
{
    return str::format("%u.%u.%u.%u/%u",
                       (addr >> 24) & 0xFF,
                       (addr >> 16) & 0xFF,
                       (addr >> 8) & 0xFF,
                       addr & 0xFF,
                       cidr);
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
            throw error_convert("ip: addr format is wrong");

        // collect digits
        do
        {
            num[i] = num[i] * 10 + (*cur++ - '0');
        } while (std::isdigit(*cur));

        if (*cur == '.')
            ++cur;

        // check if valid
        if (num[i] > 0xFF)
            throw error_convert("ip: addr number must between 0 and 255");
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
    }

    // CIDR notation
    cidr = 32;

    if (*cur == '/')
    {
        std::uint32_t tmp = 0;

        while (std::isdigit(*++cur))
            tmp = tmp * 10 + (*cur - '0');

        if (tmp > 32)
            throw error_convert("ip: CIDR prefix must less than 32");

        cidr = static_cast<uint8_t>(tmp);
    }

    return val;
}


// -----------------------------------------------------------------------------
// address_v6