/**
 * Created by Jian Chen
 * @since  2015.08.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 * @link   https://en.wikipedia.org/wiki/Classful_network
 */
#include "ip_addr.h"
#include "tool/log.h"
#include <bitset>
#include <cctype>

using namespace chen;
using namespace chen::ip;

// -----------------------------------------------------------------------------
// address


// -----------------------------------------------------------------------------
// address v4
address_v4::address_v4(std::uint32_t addr, std::uint8_t subnet)
{
    address_v4::assign(addr, subnet);
}

address_v4::address_v4(const std::string &addr, std::uint8_t subnet)
{
    address_v4::assign(addr, subnet);
}

// override
bool address_v4::empty() const
{
    return !this->_addr;
}

bool address_v4::is_loopback() const
{
    // address block 127.0.0.0/8
    return (this->_addr & 0xFF000000) == 0x7F000000;
}

bool address_v4::is_broadcast() const
{
    // host bits are 1
    std::uint32_t broadcast = (this->_addr | this->wildcard());
    return broadcast == this->_addr;
}

bool address_v4::is_multicast() const
{
    // leading: 1110, range: 224.0.0.0 ~ 239.255.255.255
    return (this->_addr & 0xF0000000) == 0xE0000000;
}

std::string address_v4::str() const
{
    return address_v4::to_string(this->_addr);
}

// type
bool address_v4::is_class_a() const
{
    // leading: 0, network: 8, range: 0.0.0.0 ~ 127.255.255.255
    return (this->_addr & 0x80000000) == 0;
}

bool address_v4::is_class_b() const
{
    // leading: 10, network: 16, range: 128.0.0.0 ~ 191.255.255.255
    return (this->_addr & 0xC0000000) == 0x80000000;
}

bool address_v4::is_class_c() const
{
    // leading: 110, network: 24, range: 192.0.0.0 ~ 223.255.255.255
    return (this->_addr & 0xE0000000) == 0xC0000000;
}

// raw
std::uint32_t address_v4::addr() const
{
    return this->_addr;
}

std::uint32_t address_v4::mask() const
{
    // if mask is valid then return mask
    // if mask is empty then calculate it
    if (this->_mask)
        return this->_mask;
    else if (this->is_class_a())
        return 0xFF000000;
    else if (this->is_class_b())
        return 0xFFFF0000;
    else if (this->is_class_c())
        return 0xFFFFFF00;
    else
        return 0xFFFFFFFF;
}

std::uint8_t address_v4::subnet() const
{
    std::bitset<32> bits(this->mask());
    return static_cast<std::uint8_t>(bits.count());
}

std::uint32_t address_v4::wildcard() const
{
    return this->mask() ^ 0xFFFFFFFF;
}

// network
address_v4 address_v4::network() const
{
    return address_v4(this->_addr & this->mask(), this->subnet());
}

address_v4 address_v4::host_min() const
{
    return address_v4((this->_addr & this->mask()) | 0x00000001, this->subnet());
}

address_v4 address_v4::host_max() const
{
    return address_v4((this->_addr | ~this->mask()) & 0xFFFFFFFE, this->subnet());
}

address_v4 address_v4::broadcast() const
{
    return address_v4(this->_addr | this->wildcard(), this->subnet());
}

// assign
void address_v4::assign(std::uint32_t addr, std::uint8_t subnet)
{
    this->_addr = addr;

    if (subnet && (subnet < 32))
    {
        auto move = 32 - subnet;
        this->_mask = 0xFFFFFFFF >> move << move;
    }
}

void address_v4::assign(const std::string &addr, std::uint8_t subnet)
{
    this->assign(address_v4::to_integer(addr), subnet);
}

// convert
std::string address_v4::to_string(std::uint32_t addr)
{
    return  std::to_string((addr >> 24) & 0xFF) + "." +
            std::to_string((addr >> 16) & 0xFF) + "." +
            std::to_string((addr >> 8) & 0xFF) + "." +
            std::to_string((addr) & 0xFF);
}

std::uint32_t address_v4::to_integer(const std::string &addr)
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
            break;
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
                break;
            }

            // find the dot character
            num[++idx] = val;

            // forward the character or break
            if (one == '\0')
                break;
            else
                one = *++ptr;
        }
        else
        {
            // find finish
            break;
        }
    }

    // analyse the digits
    switch (idx)
    {
        case 0:
            // 127 -> 127.0.0.0, 127 treated as the high 8 bits
            if (num[0] > 0xFF)
                return 0;

            return num[0] << 24;

        case 1:
            // 127.1 -> 127.0.0.1, 1 treated as 24 bits
            if ((num[0] > 0xFF) || (num[1] > 0xFF))
                return 0;

            return (num[0] << 24) | num[1];

        case 2:
            // 1.2.3 -> 1.2.0.3, 3 treated as 16 bits
            if ((num[0] > 0xFF) || (num[1] > 0xFF) || (num[2] > 0xFF))
                return 0;

            return (num[0] << 24) | (num[1] << 16) | num[2];

        case 3:
            // 8.8.8.8 -> 8.8.8.8
            if ((num[0] > 0xFF) || (num[1] > 0xFF) || (num[2] > 0xFF) || (num[3] > 0xFF))
                return 0;

            return (num[0] << 24) | (num[1] << 16) | (num[2] << 8) | num[3];

        default:
            return 0;
    }
}

address_v4 address_v4::loopback()
{
    return address_v4("127.0.0.1", 8);
}


// -----------------------------------------------------------------------------
// address v6