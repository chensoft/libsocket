/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.h"
#include "dns_codec.h"

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

void Unknown::setData(const std::uint8_t *data, std::size_t size)
{
    this->rdlength = static_cast<std::uint16_t>(size);

    this->rdata.clear();
    this->rdata.insert(this->rdata.begin(), data, data + size);
}


// -----------------------------------------------------------------------------
// A
std::vector<std::uint8_t> A::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->address, store);
    return std::move(store);
}

void A::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->address, data, size);
}


// -----------------------------------------------------------------------------
// NS
std::vector<std::uint8_t> NS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->nsdname, true, store);
    return std::move(store);
}

void NS::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->nsdname, true, data, size);
}


// -----------------------------------------------------------------------------
// CNAME
std::vector<std::uint8_t> CNAME::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->cname, true, store);
    return std::move(store);
}

void CNAME::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->cname, true, data, size);
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

void SOA::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->mname, true, data, size);
    codec::unpack(this->rname, true, data, size);
    codec::unpack(this->serial, data, size);
    codec::unpack(this->refresh, data, size);
    codec::unpack(this->retry, data, size);
    codec::unpack(this->expire, data, size);
}


// -----------------------------------------------------------------------------
// PTR
std::vector<std::uint8_t> PTR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->ptrdname, true, store);
    return std::move(store);
}

void PTR::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->ptrdname, true, data, size);
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

void MX::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->preference, data, size);
    codec::unpack(this->exchange, true, data, size);
}


// -----------------------------------------------------------------------------
// TXT
std::vector<std::uint8_t> TXT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->txt_data, false, store);
    return std::move(store);
}

void TXT::setData(const std::uint8_t *data, std::size_t size)
{
    codec::unpack(this->txt_data, false, data, size);
}