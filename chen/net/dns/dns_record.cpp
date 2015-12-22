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
// MD
std::vector<std::uint8_t> MD::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->madname, true, store);
    return std::move(store);
}

std::size_t MD::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->madname, true, data, size);
}


// -----------------------------------------------------------------------------
// MF
std::vector<std::uint8_t> MF::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->madname, true, store);
    return std::move(store);
}

std::size_t MF::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->madname, true, data, size);
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
    temp += codec::unpack(this->rname, true, data + temp, size - temp);
    temp += codec::unpack(this->serial, data + temp, size - temp);
    temp += codec::unpack(this->refresh, data + temp, size - temp);
    temp += codec::unpack(this->retry, data + temp, size - temp);
    temp += codec::unpack(this->expire, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// MB
std::vector<std::uint8_t> MB::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->madname, true, store);
    return std::move(store);
}

std::size_t MB::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->madname, true, data, size);
}


// -----------------------------------------------------------------------------
// MG
std::vector<std::uint8_t> MG::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->mgmname, true, store);
    return std::move(store);
}

std::size_t MG::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->mgmname, true, data, size);
}


// -----------------------------------------------------------------------------
// MR
std::vector<std::uint8_t> MR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->newname, true, store);
    return std::move(store);
}

std::size_t MR::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->newname, true, data, size);
}


// -----------------------------------------------------------------------------
// WKS
std::vector<std::uint8_t> WKS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->address, store);
    codec::pack(this->protocol, store);
    store.insert(store.begin(), this->bitmap.begin(), this->bitmap.end());
    return std::move(store);
}

std::size_t WKS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->address, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);

    this->bitmap.clear();
    this->bitmap.insert(this->bitmap.begin(), data + temp, data + size);

    return size;
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
// HINFO
std::vector<std::uint8_t> HINFO::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->cpu, false, store);
    codec::pack(this->os, false, store);
    return std::move(store);
}

std::size_t HINFO::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->cpu, false, data, size);
    temp += codec::unpack(this->os, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// MINFO
std::vector<std::uint8_t> MINFO::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->rmailbx, true, store);
    codec::pack(this->emailbx, true, store);
    return std::move(store);
}

std::size_t MINFO::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->rmailbx, true, data, size);
    temp += codec::unpack(this->emailbx, true, data + temp, size - temp);
    return temp;
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
    temp += codec::unpack(this->exchange, true, data + temp, size - temp);
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