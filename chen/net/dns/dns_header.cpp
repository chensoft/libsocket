/**
 * Created by Jian Chen
 * @since  2015.11.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_header.h"

#include <random>
#include <chrono>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// header

// filed value
std::int16_t header::id() const
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
    return static_cast<chen::dns::QR>(this->_flag & FLAG_MASK_QR);
}

chen::dns::OPCODE header::opcode() const
{
    return static_cast<chen::dns::OPCODE>(this->_flag & FLAG_MASK_OPCODE);
}

bool header::authoritative() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_AA);
}

bool header::truncation() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_TC);
}

bool header::recursionDesired() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_RD);
}

bool header::recursionAvailable() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_RA);
}

chen::dns::Z header::zero() const
{
    return static_cast<chen::dns::Z>(this->_flag & FLAG_MASK_Z);
}

chen::dns::RCODE header::rcode() const
{
    return static_cast<chen::dns::RCODE>(this->_flag & FLAG_MASK_RCODE);
}

// set filed value
void header::setId(std::int16_t value)
{
    this->_id = value ? value : header::random();
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
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 15) & static_cast<std::uint16_t>(FLAG_MASK_QR);
    this->_flag |= tmp;
}

void header::setOpcode(chen::dns::OPCODE value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 11) & static_cast<std::uint16_t>(FLAG_MASK_OPCODE);
    this->_flag |= tmp;
}

void header::setAuthoritative(bool value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 10) & static_cast<std::uint16_t>(FLAG_MASK_AA);
    this->_flag |= tmp;
}

void header::setTruncation(bool value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 9) & static_cast<std::uint16_t>(FLAG_MASK_TC);
    this->_flag |= tmp;
}

void header::setRecursionDesired(bool value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 8) & static_cast<std::uint16_t>(FLAG_MASK_RD);
    this->_flag |= tmp;
}

void header::setRecursionAvailable(bool value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 7) & static_cast<std::uint16_t>(FLAG_MASK_RA);
    this->_flag |= tmp;
}

void header::setZero(chen::dns::Z value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 4) & static_cast<std::uint16_t>(FLAG_MASK_Z);
    this->_flag |= tmp;
}

void header::setRcode(chen::dns::RCODE value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 0) & static_cast<std::uint16_t>(FLAG_MASK_RCODE);
    this->_flag |= tmp;
}

// random
std::int16_t header::random()
{
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<std::int16_t> uniform(1, static_cast<std::int16_t>(0xFFFF));

    auto high = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return uniform(engine) ^ static_cast<int16_t>(high);
}


// -----------------------------------------------------------------------------
// question

// get filed value
std::string question::qname() const
{
    return this->_qname;
}

chen::dns::RRType question::qtype() const
{
    return this->_qtype;
}

chen::dns::RRClass question::qclass() const
{
    return this->_qclass;
}

// set field value
void question::setQname(const std::string &value)
{
    this->_qname = value;
}

void question::setQtype(chen::dns::RRType value)
{
    this->_qtype = value;
}

void question::setQclass(chen::dns::RRClass value)
{
    this->_qclass = value;
}