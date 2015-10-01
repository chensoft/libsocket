/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_packet.h"
#include <random>
#include <chrono>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// header

// filed value
std::uint16_t header::id() const
{
    return this->_id;
}

std::uint16_t header::flag() const
{
    return this->_flag;
}

std::uint16_t header::qdcount() const
{
    return this->_qdcount;
}

std::uint16_t header::ancount() const
{
    return this->_ancount;
}

std::uint16_t header::nscount() const
{
    return this->_nscount;
}

std::uint16_t header::arcount() const
{
    return this->_arcount;
}

// flag value
chen::dns::QR header::qr() const
{
    return static_cast<chen::dns::QR>(this->_flag & 0b0000000000000001);
}

chen::dns::OPCODE header::opcode() const
{
    return static_cast<chen::dns::OPCODE>(this->_flag & 0b0000000000011110);
}

chen::dns::AA header::aa() const
{
    return static_cast<chen::dns::AA>(this->_flag & 0b0000000000100000);
}

chen::dns::TC header::tc() const
{
    return static_cast<chen::dns::TC>(this->_flag & 0b0000000001000000);
}

chen::dns::RD header::rd() const
{
    return static_cast<chen::dns::RD>(this->_flag & 0b0000000010000000);
}

chen::dns::RA header::ra() const
{
    return static_cast<chen::dns::RA>(this->_flag & 0b0000000100000000);
}

chen::dns::Z header::z() const
{
    return static_cast<chen::dns::Z>(this->_flag & 0b0000111000000000);
}

chen::dns::RCODE header::rcode() const
{
    return static_cast<chen::dns::RCODE>(this->_flag & 0b1111000000000000);
}

// set filed value
void header::setId(std::uint16_t value)
{
    this->_id = value ? value : this->random();
}

void header::setFlag(std::uint16_t value)
{
    this->_flag = value;
}

void header::setQdcount(std::uint16_t value)
{
    this->_qdcount = value;
}

void header::setAncount(std::uint16_t value)
{
    this->_ancount = value;
}

void header::setNscount(std::uint16_t value)
{
    this->_nscount = value;
}

void header::setArcount(std::uint16_t value)
{
    this->_arcount = value;
}

// set flag value
void header::setQr(chen::dns::QR value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 0) & static_cast<std::uint16_t>(0b0000000000000001);
    this->_flag |= tmp;
}

void header::setOpcode(chen::dns::OPCODE value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 1) & static_cast<std::uint16_t>(0b0000000000011110);
    this->_flag |= tmp;
}

void header::setAa(chen::dns::AA value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 5) & static_cast<std::uint16_t>(0b0000000000100000);
    this->_flag |= tmp;
}

void header::setTc(chen::dns::TC value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 6) & static_cast<std::uint16_t>(0b0000000001000000);
    this->_flag |= tmp;
}

void header::setRd(chen::dns::RD value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 7) & static_cast<std::uint16_t>(0b0000000010000000);
    this->_flag |= tmp;
}

void header::setRa(chen::dns::RA value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 8) & static_cast<std::uint16_t>(0b0000000100000000);
    this->_flag |= tmp;
}

void header::setZ(chen::dns::Z value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 9) & static_cast<std::uint16_t>(0b0000111000000000);
    this->_flag |= tmp;
}

void header::setRcode(chen::dns::RCODE value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 12) & static_cast<std::uint16_t>(0b1111000000000000);
    this->_flag |= tmp;
}

// random
std::uint16_t header::random() const
{
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<std::uint16_t> uniform(1, 0xFFFF);

    auto high = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return uniform(engine) ^ static_cast<uint16_t>(high);
}