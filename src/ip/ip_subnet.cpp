/**
 * Created by Jian Chen
 * @since  2016.08.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_subnet.hpp>
#include <socket/ip/ip_error.hpp>
#include <bitset>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// subnet_v4
subnet_v4::subnet_v4(const std::string &addr)
{
    this->assign(addr);
}

subnet_v4::subnet_v4(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

subnet_v4::subnet_v4(const std::string &addr, const std::string &mask)
{
    this->assign(addr, mask);
}

subnet_v4::subnet_v4(std::uint32_t addr)
{
    this->assign(addr);
}

subnet_v4::subnet_v4(std::uint32_t addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

subnet_v4::subnet_v4(std::uint32_t addr, const std::string &mask)
{
    this->assign(addr, mask);
}

std::shared_ptr<chen::ip::address> subnet_v4::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

// assignment
void subnet_v4::assign(const std::string &addr)
{
    this->_addr = address_v4::toInteger(addr, this->_cidr);
}

void subnet_v4::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = address_v4::toInteger(addr);
    this->_cidr = cidr;

    if (this->_cidr > 32)
        throw error_subnet("ipv4: CIDR prefix must less than 32");
}

void subnet_v4::assign(const std::string &addr, const std::string &mask)
{
    this->_addr = address_v4::toInteger(addr);
    this->_cidr = static_cast<std::uint8_t>(std::bitset<32>(address_v4::toInteger(mask)).count());
}

void subnet_v4::assign(std::uint32_t addr)
{
    address_v4::assign(addr);
    this->_cidr = 32;
}

void subnet_v4::assign(std::uint32_t addr, std::uint8_t cidr)
{
    address_v4::assign(addr);
    this->_cidr = cidr;

    if (this->_cidr > 32)
        throw error_subnet("ipv4: CIDR prefix must less than 32");
}

void subnet_v4::assign(std::uint32_t addr, const std::string &mask)
{
    this->_addr = addr;
    this->_cidr = static_cast<std::uint8_t>(std::bitset<32>(address_v4::toInteger(mask)).count());
}

address& subnet_v4::operator=(const std::string &addr)
{
    return address_v4::operator=(addr);
}

address& subnet_v4::operator=(std::uint32_t addr)
{
    return address_v4::operator=(addr);
}

// representation
std::string subnet_v4::str() const
{
    return address_v4::toString(this->_addr, this->_cidr);
}

const std::uint8_t& subnet_v4::cidr() const
{
    // @see rfc1519
    return this->_cidr;
}

std::uint8_t& subnet_v4::cidr()
{
    return this->_cidr;
}

std::uint32_t subnet_v4::netmask() const
{
    // @see rfc1878
    return 0xFFFFFFFFu << (32 - this->_cidr);
}

std::uint32_t subnet_v4::wildcard() const
{
    // @link https://en.wikipedia.org/wiki/Wildcard_mask
    return ~this->netmask();
}

// network
subnet_v4 subnet_v4::network() const
{
    return subnet_v4(this->_addr & this->netmask(), this->_cidr);
}

subnet_v4 subnet_v4::minhost() const
{
    return subnet_v4((this->_addr & this->netmask()) | 0x00000001, this->_cidr);
}

subnet_v4 subnet_v4::maxhost() const
{
    return subnet_v4((this->_addr | this->wildcard()) & 0xFFFFFFFE, this->_cidr);
}

subnet_v4 subnet_v4::broadcast() const
{
    return subnet_v4(this->_addr | this->wildcard(), this->_cidr);
}

// hosts
std::size_t subnet_v4::hosts() const
{
    return this->maxhost().addr() - this->minhost().addr() + 1;
}

// special
bool subnet_v4::isBroadcast() const
{
    // host bits are 1
    return (this->_addr | this->wildcard()) == this->_addr;
}

// operator
bool subnet_v4::operator==(const address &o) const
{
    const subnet_v4 &a = dynamic_cast<const subnet_v4&>(o);
    return (this->_addr == a._addr) && (this->_cidr == a._cidr);
}

bool subnet_v4::operator<(const address &o) const
{
    const subnet_v4 &a = dynamic_cast<const subnet_v4&>(o);
    return (this->_addr == a._addr) ? this->_cidr < a._cidr : this->_addr < a._addr;
}

bool subnet_v4::operator<=(const address &o) const
{
    const subnet_v4 &a = dynamic_cast<const subnet_v4&>(o);
    return (this->_addr == a._addr) ? this->_cidr <= a._cidr : this->_addr <= a._addr;
}


// -----------------------------------------------------------------------------
// subnet_v6
subnet_v6::subnet_v6(const std::string &addr)
{
    this->assign(addr);
}

subnet_v6::subnet_v6(const std::string &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

subnet_v6::subnet_v6(const std::array<std::uint8_t, 16> &addr)
{
    this->assign(addr);
}

subnet_v6::subnet_v6(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr)
{
    this->assign(addr, cidr);
}

subnet_v6::subnet_v6(std::array<std::uint8_t, 16> &&addr)
{
    this->assign(std::move(addr));
}

subnet_v6::subnet_v6(std::array<std::uint8_t, 16> &&addr, std::uint8_t cidr)
{
    this->assign(std::move(addr), cidr);
}

std::shared_ptr<chen::ip::address> subnet_v6::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

// assignment
void subnet_v6::assign(const std::string &addr)
{
    this->_addr = address_v6::toBytes(addr, this->_cidr);
}

void subnet_v6::assign(const std::string &addr, std::uint8_t cidr)
{
    this->_addr = std::move(address_v6::toBytes(addr));
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw error_subnet("ipv6: CIDR prefix must less than 128");
}

void subnet_v6::assign(const std::array<std::uint8_t, 16> &addr)
{
    address_v6::assign(addr);
    this->_cidr = 128;
}

void subnet_v6::assign(const std::array<std::uint8_t, 16> &addr, std::uint8_t cidr)
{
    this->_addr = addr;
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw error_subnet("ipv6: CIDR prefix must less than 128");
}

void subnet_v6::assign(std::array<std::uint8_t, 16> &&addr)
{
    address_v6::assign(std::move(addr));
    this->_cidr = 128;
}

void subnet_v6::assign(std::array<std::uint8_t, 16> &&addr, std::uint8_t cidr)
{
    this->_addr = std::move(addr);
    this->_cidr = cidr;

    if (this->_cidr > 128)
        throw error_subnet("ipv6: CIDR prefix must less than 128");
}

address& subnet_v6::operator=(const std::string &addr)
{
    return address_v6::operator=(addr);
}

address& subnet_v6::operator=(const std::array<std::uint8_t, 16> &addr)
{
    return address_v6::operator=(addr);
}

address& subnet_v6::operator=(std::array<std::uint8_t, 16> &&addr)
{
    return address_v6::operator=(std::move(addr));
}

// representation
std::string subnet_v6::str() const
{
    return address_v6::toString(this->_addr, this->_cidr);
}

const std::uint8_t& subnet_v6::cidr() const
{
    return this->_cidr;
}

std::uint8_t& subnet_v6::cidr()
{
    return this->_cidr;
}

std::array<std::uint8_t, 16> subnet_v6::netmask() const
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

std::array<std::uint8_t, 16> subnet_v6::wildcard() const
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
subnet_v6 subnet_v6::network() const
{
    std::array<std::uint8_t, 16> ret{};
    std::array<std::uint8_t, 16> mask = this->netmask();

    for (std::size_t i = 0, len = ret.size(); i < len; ++i)
        ret[i] = this->_addr[i] & mask[i];

    return subnet_v6(ret, this->_cidr);
}

subnet_v6 subnet_v6::minhost() const
{
    // IPv6 host begins with 0
    return this->network();
}

subnet_v6 subnet_v6::maxhost() const
{
    // IPv6 host ends with 1
    std::array<std::uint8_t, 16> ret{};
    std::array<std::uint8_t, 16> mask = this->wildcard();

    for (std::size_t i = 0, len = ret.size(); i < len; ++i)
        ret[i] = this->_addr[i] | mask[i];

    return subnet_v6(ret, this->_cidr);
}

// operator
bool subnet_v6::operator==(const address &o) const
{
    const subnet_v6 &a = dynamic_cast<const subnet_v6&>(o);
    return (this->_addr == a._addr) && (this->_cidr == a._cidr);
}

bool subnet_v6::operator<(const address &o) const
{
    const subnet_v6 &a = dynamic_cast<const subnet_v6&>(o);
    return (this->_addr == a._addr) ? this->_cidr < a._cidr : this->_addr < a._addr;
}

bool subnet_v6::operator<=(const address &o) const
{
    const subnet_v6 &a = dynamic_cast<const subnet_v6&>(o);
    return (this->_addr == a._addr) ? this->_cidr <= a._cidr : this->_addr <= a._addr;
}