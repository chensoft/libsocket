/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.h"
#include "dns_error.h"
#include "dns_tool.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR

// field value
std::string RR::name() const
{
    return this->_name;
}

chen::dns::RRType RR::rrtype() const
{
    return this->_rrtype;
}

chen::dns::RRClass RR::rrclass() const
{
    return this->_rrclass;
}

std::int32_t RR::ttl() const
{
    return this->_ttl;
}

std::uint16_t RR::rdlength() const
{
    return this->_rdlength;
}

std::vector<std::uint8_t> RR::rdata() const
{
    return this->_rdata;
}

// set field value
void RR::setName(const std::string &value)
{
    this->_name = value;
}

void RR::setRrtype(chen::dns::RRType value)
{
    this->_rrtype = value;
}

void RR::setRrclass(chen::dns::RRClass value)
{
    this->_rrclass = value;
}

void RR::setTtl(std::int32_t value)
{
    this->_ttl = value;
}

void RR::setRdlength(std::uint16_t value)
{
    this->_rdlength = value;
}

void RR::setRdata(const std::uint8_t *data, std::size_t size)
{
    this->_rdata.assign(data, data + size);
}

// assign
std::size_t RR::assign(const std::uint8_t *cur_data, std::size_t cur_size,
                       const std::uint8_t *org_data, std::size_t org_size)
{
    auto ptr = cur_data;
    auto len = cur_size;

    // name
    if (*ptr & 0b11000000)  // detect compression
    {
        std::size_t offset = static_cast<std::size_t>((*ptr & 0b00110000 << 8) + *(ptr + 1));

        this->_name = pack::binaryToName(org_data + offset, org_size - offset);

        ptr += 2;
        len -= 2;
    }
    else
    {
        this->_name = pack::binaryToName(ptr, len);

        ptr += this->_name.size() + 1;
        len  = cur_size - (ptr - cur_data);
    }

    // type
    if (len < 2)
        throw error_size("RR assign type size is not enough");

    this->_rrtype = static_cast<chen::dns::RRType>((*ptr << 8) + *(ptr + 1));

    ptr += 2;
    len -= 2;

    // class
    if (len < 2)
        throw error_size("RR assign class size is not enough");

    this->_rrclass = static_cast<chen::dns::RRClass>((*ptr << 8) + *(ptr + 1));

    ptr += 2;
    len -= 2;

    // ttl
    if (len < 4)
        throw error_size("RR assign ttl size is not enough");

    this->_ttl = static_cast<std::int32_t>((*ptr << 24) + (*(ptr + 1) << 16) + (*(ptr + 2) << 8) + *(ptr + 3));

    ptr += 4;
    len -= 4;

    // length
    if (len < 2)
        throw error_size("RR assign length size is not enough");

    this->_rdlength = static_cast<std::uint16_t>((*ptr << 8) + *(ptr + 1));

    ptr += 2;
    len -= 2;

    // data
    if (len < this->_rdlength)
        throw error_size("RR assign data size is not enough");

    this->_rdata.clear();
    this->_rdata.assign(ptr, ptr + this->_rdlength);

    len -= this->_rdlength;

    return cur_size - len;
}