/**
 * Created by Jian Chen
 * @since  2015.08.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   https://en.wikipedia.org/wiki/Classful_network
 */
#include <chen/net/ip/ip_addr.hpp>
#include <chen/net/ip/ip_error.hpp>
#include <bitset>
#include <cctype>
#include <chen/base/str.hpp>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// address
address::address(std::uint32_t addr, std::uint8_t subnet)
{
    address::assign(addr, subnet);
}

address::address(const std::string &addr, std::uint8_t subnet)
{
    address::assign(addr, subnet);
}

// status
bool address::empty() const
{
    return !this->_addr;
}

bool address::isLoopback() const
{
    // address block 127.0.0.0/8
    return (this->_addr & 0xFF000000) == 0x7F000000;
}

bool address::isBroadcast() const
{
    // host bits are 1
    std::uint32_t broadcast = (this->_addr | this->wildcard());
    return broadcast == this->_addr;
}

bool address::isMulticast() const
{
    // leading: 1110, range: 224.0.0.0 ~ 239.255.255.255
    return (this->_addr & 0xF0000000) == 0xE0000000;
}

// type
bool address::isClassA() const
{
    // leading: 0, network: 8, range: 0.0.0.0 ~ 127.255.255.255
    return (this->_addr & 0x80000000) == 0;
}

bool address::isClassB() const
{
    // leading: 10, network: 16, range: 128.0.0.0 ~ 191.255.255.255
    return (this->_addr & 0xC0000000) == 0x80000000;
}

bool address::isClassC() const
{
    // leading: 110, network: 24, range: 192.0.0.0 ~ 223.255.255.255
    return (this->_addr & 0xE0000000) == 0xC0000000;
}

// raw
std::string address::str() const
{
    return address::toString(this->_addr);
}

std::uint32_t address::addr() const
{
    return this->_addr;
}

std::uint32_t address::mask() const
{
    // if mask is valid then return mask
    // if mask is empty then calculate it
    if (this->_mask)
        return this->_mask;
    else if (this->isClassA())
        return 0xFF000000;
    else if (this->isClassB())
        return 0xFFFF0000;
    else if (this->isClassC())
        return 0xFFFFFF00;
    else
        return 0xFFFFFFFF;
}

std::string address::full() const
{
    return this->str() + "/" + std::to_string(this->subnet());
}

std::uint8_t address::subnet() const
{
    std::bitset<32> bits(this->mask());
    return static_cast<std::uint8_t>(bits.count());
}

std::uint32_t address::wildcard() const
{
    return this->mask() ^ 0xFFFFFFFF;
}

// network
address address::network() const
{
    return address(this->_addr & this->mask(), this->subnet());
}

address address::hostMin() const
{
    return address((this->_addr & this->mask()) | 0x00000001, this->subnet());
}

address address::hostMax() const
{
    return address((this->_addr | ~this->mask()) & 0xFFFFFFFE, this->subnet());
}

address address::broadcast() const
{
    return address(this->_addr | this->wildcard(), this->subnet());
}

// assign
void address::assign(std::uint32_t addr, std::uint8_t subnet)
{
    this->_addr = addr;

    // subnet range: [8, 30]
    if (((subnet >= 8) && (subnet <= 30)))
    {
        auto   move = 32 - subnet;
        this->_mask = 0xFFFFFFFF >> move << move;
    }
    else if (!subnet)
    {
        this->_mask = subnet;
    }
    else
    {
        throw error_invalid("ip: subnet range error");
    }
}

void address::assign(const std::string &addr, std::uint8_t subnet)
{
    this->assign(address::toInteger(addr), subnet);
}

// convert
std::string address::toString(std::uint32_t addr)
{
    return str::format("%u.%u.%u.%u",
                       (addr >> 24) & 0xFF,
                       (addr >> 16) & 0xFF,
                       (addr >> 8) & 0xFF,
                       (addr) & 0xFF);
}

std::uint32_t address::toInteger(const std::string &addr)
{
    int idx = -1;
    std::uint32_t val = 0;
    std::uint32_t num[4] = {0, 0, 0, 0};

    auto ptr = addr.c_str();  // ending with '\0'
    char one = *ptr;

    // retrieve all digits
    while (true)
    {
        // reset value
        val = 0;

        // check first
        if (!std::isdigit(one))
        {
            // occur invalid character
            idx = -1;

            if (addr.empty())
                break;
            else
                throw error_convert("ip: addr has invalid character");
        }

        // retrieve a int until '.'
        while (true)
        {
            if (std::isdigit(one))
            {
                // improve the speed to translate string to digit
                val = val * 10 + (one - '0');
                one = *++ptr;
            }
            else
            {
                break;
            }
        }

        // collect until '.' or '\0'
        if ((one == '.') || (one == '\0'))
        {
            // range overflow
            if (idx >= 3)
            {
                idx = -1;
                throw error_convert("ip: addr has too many numbers");
            }

            // find the dot character
            num[++idx] = val;

            // forward the character or break
            if (one == '\0')
                break;
            else
                one = *++ptr;
        }
    }

    // analyse the digits
    switch (idx)
    {
        case 0:
            // 127 -> 127.0.0.0, 127 treated as the high 8 bits
            if (num[0] > 0xFF)
                throw error_convert("ip: addr number must between 0 and 255");

            return num[0] << 24;

        case 1:
            // 127.1 -> 127.0.0.1, 1 treated as 24 bits
            if ((num[0] > 0xFF) || (num[1] > 0xFF))
                throw error_convert("ip: addr number must between 0 and 255");

            return (num[0] << 24) | num[1];

        case 2:
            // 1.2.3 -> 1.2.0.3, 3 treated as 16 bits
            if ((num[0] > 0xFF) || (num[1] > 0xFF) || (num[2] > 0xFF))
                throw error_convert("ip: addr number must between 0 and 255");

            return (num[0] << 24) | (num[1] << 16) | num[2];

        case 3:
            // 8.8.8.8 -> 8.8.8.8
            if ((num[0] > 0xFF) || (num[1] > 0xFF) || (num[2] > 0xFF) || (num[3] > 0xFF))
                throw error_convert("ip: addr number must between 0 and 255");

            return (num[0] << 24) | (num[1] << 16) | (num[2] << 8) | num[3];

        default:
            return 0;
    }
}

address address::loopback()
{
    // 127.0.0.1
    return address(0x7F000001, 8);
}

// operator
bool chen::ip::operator==(const address &a, const address &b)
{
    return a.addr() == b.addr();
}

bool chen::ip::operator!=(const address &a, const address &b)
{
    return !(a == b);
}

bool chen::ip::operator<(const address &a, const address &b)
{
    return a.addr() < b.addr();
}

bool chen::ip::operator>(const address &a, const address &b)
{
    return b < a;
}

bool chen::ip::operator<=(const address &a, const address &b)
{
    return !(b < a);
}

bool chen::ip::operator>=(const address &a, const address &b)
{
    return !(a < b);
}