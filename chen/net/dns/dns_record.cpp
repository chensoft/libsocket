/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.h"
#include "dns_codec.h"
#include "dns_error.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR
RR::~RR()
{

}


// -----------------------------------------------------------------------------
// Unknown
std::vector<std::uint8_t> Unknown::data() const
{
    return this->rdata;
}

std::size_t Unknown::setData(const std::uint8_t *data, std::size_t size)
{
    if (size < 2)
        throw error_size("record data size is not enough, require 2 bytes");

    // rdlength
    std::uint16_t rdlength = static_cast<std::uint16_t>((data[0] << 8) + data[1]);

    size -= 2;

    if (size < rdlength)
        throw error_size("record data size is too small");

    // rdata
    this->rdlength = rdlength;

    this->rdata.clear();
    this->rdata.insert(this->rdata.begin(), data + 2, data + rdlength);

    return rdlength + 2;
}


// -----------------------------------------------------------------------------
// A
std::vector<std::uint8_t> A::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->address, store);
    return std::move(store);
}

std::size_t A::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->address, data, size);
}


// -----------------------------------------------------------------------------
// NS
std::vector<std::uint8_t> NS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->nsdname, true, store);
    return std::move(store);
}

std::size_t NS::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->nsdname, true, data, size);
}


// -----------------------------------------------------------------------------
// CNAME
std::vector<std::uint8_t> CNAME::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->cname, true, store);
    return std::move(store);
}

std::size_t CNAME::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->cname, true, data, size);
}


// -----------------------------------------------------------------------------
// SOA
std::vector<std::uint8_t> SOA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->mname, true, store);
    codec::pack(this->rname, true, store);
    codec::pack(this->serial, store);
    codec::pack(this->refresh, store);
    codec::pack(this->retry, store);
    codec::pack(this->expire, store);
    return std::move(store);
}

std::size_t SOA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->mname, true, data, size);
    temp += codec::unpack(this->rname, true, data, size);
    temp += codec::unpack(this->serial, data, size);
    temp += codec::unpack(this->refresh, data, size);
    temp += codec::unpack(this->retry, data, size);
    temp += codec::unpack(this->expire, data, size);
    return temp;
}


// -----------------------------------------------------------------------------
// PTR
std::vector<std::uint8_t> PTR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->ptrdname, true, store);
    return std::move(store);
}

std::size_t PTR::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->ptrdname, true, data, size);
}


// -----------------------------------------------------------------------------
// MX
std::vector<std::uint8_t> MX::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->exchange, true, store);
    return std::move(store);
}

std::size_t MX::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->exchange, true, data, size);
    return temp;
}


// -----------------------------------------------------------------------------
// TXT
std::vector<std::uint8_t> TXT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->txt_data, false, store);
    return std::move(store);
}

std::size_t TXT::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->txt_data, false, data, size);
}