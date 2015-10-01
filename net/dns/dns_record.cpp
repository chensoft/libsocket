/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR

// field value
std::string RR::name() const
{
    return this->_name;
}

std::uint16_t RR::rrtype() const
{
    return this->_rrtype;
}

std::uint16_t RR::rrclass() const
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

// set field value
void RR::setName(const std::string &value)
{
    this->_name = value;
}

void RR::setRrtype(std::uint16_t value)
{
    this->_rrtype = value;
}

void RR::setRrclass(std::uint16_t value)
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