/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.h"
#include "dns_codec.h"
#include "dns_error.h"
#include <chen/tool/str.h>

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
    this->rdata.insert(this->rdata.cbegin(), data + 2, data + rdlength);

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
    store.insert(store.cbegin(), this->bitmap.cbegin(), this->bitmap.cend());
    return std::move(store);
}

std::size_t WKS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->address, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);

    this->bitmap.clear();
    this->bitmap.insert(this->bitmap.cbegin(), data + temp, data + size);

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


// -----------------------------------------------------------------------------
// RP
std::vector<std::uint8_t> RP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->mbox_dname, true, store);
    codec::pack(this->txt_dname, true, store);
    return std::move(store);
}

std::size_t RP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->mbox_dname, true, data, size);
    temp += codec::unpack(this->txt_dname, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// AFSDB
std::vector<std::uint8_t> AFSDB::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->subtype, store);
    codec::pack(this->hostname, true, store);
    return std::move(store);
}

std::size_t AFSDB::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->subtype, data, size);
    temp += codec::unpack(this->hostname, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// X25
std::vector<std::uint8_t> X25::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->psdn_address, false, store);
    return std::move(store);
}

std::size_t X25::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->psdn_address, false, data, size);
}


// -----------------------------------------------------------------------------
// ISDN
std::vector<std::uint8_t> ISDN::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->isdn_address, false, store);
    return std::move(store);
}

std::size_t ISDN::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->isdn_address, false, data, size);
}


// -----------------------------------------------------------------------------
// RT
std::vector<std::uint8_t> RT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->intermediate_host, true, store);
    return std::move(store);
}

std::size_t RT::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->intermediate_host, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// NSAP
std::vector<std::uint8_t> NSAP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->nsap, false, store);
    return std::move(store);
}

std::size_t NSAP::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->nsap, false, data, size);
}


// -----------------------------------------------------------------------------
// NSAPPTR
std::vector<std::uint8_t> NSAPPTR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->ptrdname, true, store);
    return std::move(store);
}

std::size_t NSAPPTR::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->ptrdname, true, data, size);
}


// -----------------------------------------------------------------------------
// SIG
std::vector<std::uint8_t> SIG::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->typecovered, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->labels, store);
    codec::pack(this->original, store);
    codec::pack(this->expiration, store);
    codec::pack(this->inception, store);
    codec::pack(this->keytag, store);
    codec::pack(this->signer, true, store);
    codec::pack(this->signature, false, store);
    return std::move(store);
}

std::size_t SIG::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->typecovered, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->labels, data + temp, size - temp);
    temp += codec::unpack(this->original, data + temp, size - temp);
    temp += codec::unpack(this->expiration, data + temp, size - temp);
    temp += codec::unpack(this->inception, data + temp, size - temp);
    temp += codec::unpack(this->keytag, data + temp, size - temp);
    temp += codec::unpack(this->signer, true, data + temp, size - temp);
    temp += codec::unpack(this->signature, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// KEY
std::vector<std::uint8_t> KEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->protocol, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->publickey, false, store);
    return std::move(store);
}

std::size_t KEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// PX
std::vector<std::uint8_t> PX::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->map822, true, store);
    codec::pack(this->mapx400, true, store);
    return std::move(store);
}

std::size_t PX::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->map822, true, data + temp, size - temp);
    temp += codec::unpack(this->mapx400, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// GPOS
std::vector<std::uint8_t> GPOS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->longitude, false, store);
    codec::pack(this->latitude, false, store);
    codec::pack(this->altitude, false, store);
    return std::move(store);
}

std::size_t GPOS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->longitude, false, data, size);
    temp += codec::unpack(this->latitude, false, data + temp, size - temp);
    temp += codec::unpack(this->altitude, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// AAAA
std::vector<std::uint8_t> AAAA::data() const
{
    std::vector<std::uint8_t> store;
    store.insert(store.cbegin(), this->address.cbegin(), this->address.cend());
    return std::move(store);
}

std::size_t AAAA::setData(const std::uint8_t *data, std::size_t size)
{
    auto need = this->address.size();

    if (size < need)
        throw error_size(str::format("codec unpack AAAA size is not enough, require %d bytes", need));

    std::copy(data, data + need, this->address.begin());

    return need;
}


// -----------------------------------------------------------------------------
// LOC
std::vector<std::uint8_t> LOC::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->version, store);
    codec::pack(this->size, store);
    codec::pack(this->horizpre, store);
    codec::pack(this->vertpre, store);
    codec::pack(this->longitude, store);
    codec::pack(this->latitude, store);
    codec::pack(this->altitude, store);
    return std::move(store);
}

std::size_t LOC::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->version, data, size);
    temp += codec::unpack(this->size, data + temp, size - temp);
    temp += codec::unpack(this->horizpre, data + temp, size - temp);
    temp += codec::unpack(this->vertpre, data + temp, size - temp);
    temp += codec::unpack(this->longitude, data + temp, size - temp);
    temp += codec::unpack(this->latitude, data + temp, size - temp);
    temp += codec::unpack(this->altitude, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// NXT
std::vector<std::uint8_t> NXT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->next, true, store);
    store.insert(store.cbegin(), this->bitmap.cbegin(), this->bitmap.cend());
    return std::move(store);
}

std::size_t NXT::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->next, true, data, size);

    this->bitmap.clear();
    this->bitmap.insert(this->bitmap.cbegin(), data + temp, data + size);

    return size;
}