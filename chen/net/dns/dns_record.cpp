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
    codec::pack(this->nsdname, store, true);
    return std::move(store);
}

std::size_t NS::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->nsdname, data, size, true);
}


// -----------------------------------------------------------------------------
// MD
std::vector<std::uint8_t> MD::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->madname, store, true);
    return std::move(store);
}

std::size_t MD::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->madname, data, size, true);
}


// -----------------------------------------------------------------------------
// MF
std::vector<std::uint8_t> MF::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->madname, store, true);
    return std::move(store);
}

std::size_t MF::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->madname, data, size, true);
}


// -----------------------------------------------------------------------------
// CNAME
std::vector<std::uint8_t> CNAME::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->cname, store, true);
    return std::move(store);
}

std::size_t CNAME::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->cname, data, size, true);
}


// -----------------------------------------------------------------------------
// SOA
std::vector<std::uint8_t> SOA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->mname, store, true);
    codec::pack(this->rname, store, true);
    codec::pack(this->serial, store);
    codec::pack(this->refresh, store);
    codec::pack(this->retry, store);
    codec::pack(this->expire, store);
    codec::pack(this->minimum, store);
    return std::move(store);
}

std::size_t SOA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->mname, data, size, true);
    temp += codec::unpack(this->rname, data + temp, size - temp, true);
    temp += codec::unpack(this->serial, data + temp, size - temp);
    temp += codec::unpack(this->refresh, data + temp, size - temp);
    temp += codec::unpack(this->retry, data + temp, size - temp);
    temp += codec::unpack(this->expire, data + temp, size - temp);
    temp += codec::unpack(this->minimum, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// MB
std::vector<std::uint8_t> MB::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->madname, store, true);
    return std::move(store);
}

std::size_t MB::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->madname, data, size, true);
}


// -----------------------------------------------------------------------------
// MG
std::vector<std::uint8_t> MG::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->mgmname, store, true);
    return std::move(store);
}

std::size_t MG::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->mgmname, data, size, true);
}


// -----------------------------------------------------------------------------
// MR
std::vector<std::uint8_t> MR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->newname, store, true);
    return std::move(store);
}

std::size_t MR::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->newname, data, size, true);
}


// -----------------------------------------------------------------------------
// RNULL
std::vector<std::uint8_t> RNULL::data() const
{
    std::vector<std::uint8_t> store;
    std::copy(this->anything.cbegin(), this->anything.cend(), store.begin());
    return std::move(store);
}

std::size_t RNULL::setData(const std::uint8_t *data, std::size_t size)
{
    this->anything.clear();
    this->anything.insert(this->anything.cbegin(), data, data + size);

    return size;
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
    codec::pack(this->ptrdname, store, true);
    return std::move(store);
}

std::size_t PTR::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->ptrdname, data, size, true);
}


// -----------------------------------------------------------------------------
// HINFO
std::vector<std::uint8_t> HINFO::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->cpu, store, false);
    codec::pack(this->os, store, false);
    return std::move(store);
}

std::size_t HINFO::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->cpu, data, size, false);
    temp += codec::unpack(this->os, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// MINFO
std::vector<std::uint8_t> MINFO::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->rmailbx, store, true);
    codec::pack(this->emailbx, store, true);
    return std::move(store);
}

std::size_t MINFO::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->rmailbx, data, size, true);
    temp += codec::unpack(this->emailbx, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// MX
std::vector<std::uint8_t> MX::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->exchange, store, true);
    return std::move(store);
}

std::size_t MX::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->exchange, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// TXT
std::vector<std::uint8_t> TXT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->txt_data, store, false);
    return std::move(store);
}

std::size_t TXT::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->txt_data, data, size, false);
}


// -----------------------------------------------------------------------------
// RP
std::vector<std::uint8_t> RP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->mbox_dname, store, true);
    codec::pack(this->txt_dname, store, true);
    return std::move(store);
}

std::size_t RP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->mbox_dname, data, size, true);
    temp += codec::unpack(this->txt_dname, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// AFSDB
std::vector<std::uint8_t> AFSDB::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->subtype, store);
    codec::pack(this->hostname, store, true);
    return std::move(store);
}

std::size_t AFSDB::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->subtype, data, size);
    temp += codec::unpack(this->hostname, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// X25
std::vector<std::uint8_t> X25::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->psdn_address, store, false);
    return std::move(store);
}

std::size_t X25::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->psdn_address, data, size, false);
}


// -----------------------------------------------------------------------------
// ISDN
std::vector<std::uint8_t> ISDN::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->isdn_address, store, false);
    codec::pack(this->sa, store, false);
    return std::move(store);
}

std::size_t ISDN::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->isdn_address, data, size, false);
    temp += codec::unpack(this->sa, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// RT
std::vector<std::uint8_t> RT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->intermediate_host, store, true);
    return std::move(store);
}

std::size_t RT::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->intermediate_host, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// NSAP
std::vector<std::uint8_t> NSAP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->nsap, store, false);
    return std::move(store);
}

std::size_t NSAP::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->nsap, data, size, false);
}


// -----------------------------------------------------------------------------
// NSAPPTR
std::vector<std::uint8_t> NSAPPTR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->owner, store, true);
    return std::move(store);
}

std::size_t NSAPPTR::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->owner, data, size, true);
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
    codec::pack(this->signer, store, true);
    codec::pack(this->signature, store, false);
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
    temp += codec::unpack(this->signer, data + temp, size - temp, true);
    temp += codec::unpack(this->signature, data + temp, size - temp, false);
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
    codec::pack(this->publickey, store, false);
    return std::move(store);
}

std::size_t KEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// PX
std::vector<std::uint8_t> PX::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->map822, store, true);
    codec::pack(this->mapx400, store, true);
    return std::move(store);
}

std::size_t PX::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->map822, data + temp, size - temp, true);
    temp += codec::unpack(this->mapx400, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// GPOS
std::vector<std::uint8_t> GPOS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->longitude, store, false);
    codec::pack(this->latitude, store, false);
    codec::pack(this->altitude, store, false);
    return std::move(store);
}

std::size_t GPOS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->longitude, data, size, false);
    temp += codec::unpack(this->latitude, data + temp, size - temp, false);
    temp += codec::unpack(this->altitude, data + temp, size - temp, false);
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
    codec::pack(this->nextdomain, store, true);
    store.insert(store.cbegin(), this->typebitmap.cbegin(), this->typebitmap.cend());
    return std::move(store);
}

std::size_t NXT::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->nextdomain, data, size, true);

    this->typebitmap.clear();
    this->typebitmap.insert(this->typebitmap.cbegin(), data + temp, data + size);

    return size;
}


// -----------------------------------------------------------------------------
// EID
std::vector<std::uint8_t> EID::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->endpoint, store, false);
    return std::move(store);
}

std::size_t EID::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->endpoint, data, size, false);
}


// -----------------------------------------------------------------------------
// NIMLOC
std::vector<std::uint8_t> NIMLOC::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->locator, store, false);
    return std::move(store);
}

std::size_t NIMLOC::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->locator, data, size, false);
}


// -----------------------------------------------------------------------------
// SRV
std::vector<std::uint8_t> SRV::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->priority, store);
    codec::pack(this->weight, store);
    codec::pack(this->port, store);
    codec::pack(this->target, store, true);
    return std::move(store);
}

std::size_t SRV::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->priority, data, size);
    temp += codec::unpack(this->weight, data + temp, size - temp);
    temp += codec::unpack(this->port, data + temp, size - temp);
    temp += codec::unpack(this->target, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// ATMA
std::vector<std::uint8_t> ATMA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->format, store);
    codec::pack(this->address, store, false);
    return std::move(store);
}

std::size_t ATMA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->format, data, size);
    temp += codec::unpack(this->address, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// NAPTR
std::vector<std::uint8_t> NAPTR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->order, store);
    codec::pack(this->preference, store);
    codec::pack(this->flags, store, false);
    codec::pack(this->services, store, false);
    codec::pack(this->regexp, store, false);
    codec::pack(this->replacement, store, true);
    return std::move(store);
}

std::size_t NAPTR::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->order, data, size);
    temp += codec::unpack(this->preference, data + temp, size - temp);
    temp += codec::unpack(this->flags, data + temp, size - temp, false);
    temp += codec::unpack(this->services, data + temp, size - temp, false);
    temp += codec::unpack(this->regexp, data + temp, size - temp, false);
    temp += codec::unpack(this->replacement, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// KX
std::vector<std::uint8_t> KX::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->exchanger, store, true);
    return std::move(store);
}

std::size_t KX::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->exchanger, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// CERT
std::vector<std::uint8_t> CERT::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->type, store);
    codec::pack(this->keytag, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->certificate, store, false);
    return std::move(store);
}

std::size_t CERT::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->type, data, size);
    temp += codec::unpack(this->keytag, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->certificate, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// DNAME
std::vector<std::uint8_t> DNAME::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->target, store, true);
    return std::move(store);
}

std::size_t DNAME::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->target, data, size, true);
}


// -----------------------------------------------------------------------------
// SINK
std::vector<std::uint8_t> SINK::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->coding, store);
    codec::pack(this->subcoding, store);
    store.insert(store.cend(), this->sdata.cbegin(), this->sdata.cend());
    return std::move(store);
}

std::size_t SINK::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->coding, data, size);
    temp += codec::unpack(this->subcoding, data + temp, size - temp);

    this->sdata.clear();
    this->sdata.insert(this->sdata.cbegin(), data + temp, data + size);

    return size;
}


// -----------------------------------------------------------------------------
// DS
std::vector<std::uint8_t> DS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->keytag, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->digesttype, store);
    codec::pack(this->digest, store, false);
    return std::move(store);
}

std::size_t DS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// SSHFP
std::vector<std::uint8_t> SSHFP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->algorithm, store);
    codec::pack(this->fptype, store);
    codec::pack(this->fingerprint, store, false);
    return std::move(store);
}

std::size_t SSHFP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->algorithm, data, size);
    temp += codec::unpack(this->fptype, data + temp, size - temp);
    temp += codec::unpack(this->fingerprint, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// IPSECKEY
std::vector<std::uint8_t> IPSECKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->precedence, store);
    codec::pack(this->gatewaytype, store);
    codec::pack(this->algorithm, store);

    switch (static_cast<GatewayType>(this->gatewaytype))
    {
        case GatewayType::None:
            codec::pack(".", store, false);
            break;

        case GatewayType::IPv4:
            store.insert(store.cend(), this->gateway.cbegin(), this->gateway.cbegin() + 4);
            break;

        case GatewayType::IPv6:
            store.insert(store.cend(), this->gateway.cbegin(), this->gateway.cbegin() + 16);
            break;

        case GatewayType::Domain:
        {
            std::string domain(this->gateway.cbegin(), this->gateway.cend());
            codec::pack(domain, store, true);
        }
            break;
    }

    codec::pack(this->publickey, store, false);
    return std::move(store);
}

std::size_t IPSECKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->precedence, data, size);
    temp += codec::unpack(this->gatewaytype, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);

    this->gateway.clear();

    switch (static_cast<GatewayType>(this->gatewaytype))
    {
        case GatewayType::None:
            this->gateway.push_back(static_cast<std::uint8_t>('.'));
            ++temp;
            break;

        case GatewayType::IPv4:
            if (size - temp < 4)
                throw error_size(str::format("codec unpack IPSECKEY size is not enough, require 4 bytes"));

            this->gateway.insert(this->gateway.cbegin(), data + temp, data + temp + 4);

            temp += 4;
            break;

        case GatewayType::IPv6:
            if (size - temp < 16)
                throw error_size(str::format("codec unpack IPSECKEY size is not enough, require 16 bytes"));

            this->gateway.insert(this->gateway.cbegin(), data + temp, data + temp + 16);

            temp += 16;
            break;

        case GatewayType::Domain:
        {
            std::string domain;
            temp += codec::unpack(domain, data, size, true);

            this->gateway.insert(this->gateway.cbegin(), domain.cbegin(), domain.cend());
        }
            break;
    }

    temp += codec::unpack(this->publickey, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// RRSIG
std::vector<std::uint8_t> RRSIG::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->typecovered, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->labels, store);
    codec::pack(this->original, store);
    codec::pack(this->expiration, store);
    codec::pack(this->inception, store);
    codec::pack(this->keytag, store);
    codec::pack(this->signer, store, true);
    codec::pack(this->signature, store, false);
    return std::move(store);
}

std::size_t RRSIG::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->typecovered, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->labels, data + temp, size - temp);
    temp += codec::unpack(this->original, data + temp, size - temp);
    temp += codec::unpack(this->expiration, data + temp, size - temp);
    temp += codec::unpack(this->inception, data + temp, size - temp);
    temp += codec::unpack(this->keytag, data + temp, size - temp);
    temp += codec::unpack(this->signer, data + temp, size - temp, true);
    temp += codec::unpack(this->signature, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// NSEC
std::vector<std::uint8_t> NSEC::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->nextdomain, store, true);
    std::copy(this->typebitmap.cbegin(), this->typebitmap.cend(), store.end());
    return std::move(store);
}

std::size_t NSEC::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->nextdomain, data, size, true);

    this->typebitmap.clear();
    std::copy(data + temp, data + size, this->typebitmap.begin());

    return size;
}


// -----------------------------------------------------------------------------
// DNSKEY
std::vector<std::uint8_t> DNSKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->protocol, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->publickey, store, false);
    return std::move(store);
}

std::size_t DNSKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// DHCID
std::vector<std::uint8_t> DHCID::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->digest, store, false);
    return std::move(store);
}

std::size_t DHCID::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->digest, data, size, false);
}


// -----------------------------------------------------------------------------
// NSEC3
std::vector<std::uint8_t> NSEC3::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->hash, store);
    codec::pack(this->flags, store);
    codec::pack(this->iterations, store);
    codec::pack(this->saltLength, store);
    store.insert(store.cbegin(), this->salt.cbegin(), this->salt.cbegin() + this->saltLength);
    codec::pack(this->hashlength, store);
    codec::pack(this->nextowner, store, false);
    store.insert(store.cbegin(), this->typebitmap.cbegin(), this->typebitmap.cend());
    return std::move(store);
}

std::size_t NSEC3::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->hash, data, size);
    temp += codec::unpack(this->flags, data + temp, size - temp);
    temp += codec::unpack(this->iterations, data + temp, size - temp);
    temp += codec::unpack(this->saltLength, data + temp, size - temp);

    this->salt.clear();
    this->salt.insert(this->salt.cbegin(), data + temp, data + temp + this->saltLength);

    temp += codec::unpack(this->hashlength, data + temp, size - temp);
    temp += codec::unpack(this->nextowner, data + temp, size - temp, false);

    this->typebitmap.clear();
    this->typebitmap.insert(this->typebitmap.cbegin(), data + temp, data + size);

    return size;
}


// -----------------------------------------------------------------------------
// NSEC3PARAM
std::vector<std::uint8_t> NSEC3PARAM::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->hash, store);
    codec::pack(this->flags, store);
    codec::pack(this->iterations, store);
    codec::pack(this->saltLength, store);
    store.insert(store.cbegin(), this->salt.cbegin(), this->salt.cbegin() + this->saltLength);
    return std::move(store);
}

std::size_t NSEC3PARAM::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->hash, data, size);
    temp += codec::unpack(this->flags, data + temp, size - temp);
    temp += codec::unpack(this->iterations, data + temp, size - temp);
    temp += codec::unpack(this->saltLength, data + temp, size - temp);

    this->salt.clear();
    this->salt.insert(this->salt.cbegin(), data + temp, data + temp + this->saltLength);

    return temp + this->saltLength;
}


// -----------------------------------------------------------------------------
// TLSA
std::vector<std::uint8_t> TLSA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->usage, store);
    codec::pack(this->selector, store);
    codec::pack(this->matchingtype, store);
    codec::pack(this->certificate, store, false);
    return std::move(store);
}

std::size_t TLSA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->usage, data, size);
    temp += codec::unpack(this->selector, data + temp, size - temp);
    temp += codec::unpack(this->matchingtype, data + temp, size - temp);
    temp += codec::unpack(this->certificate, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// SMIMEA
std::vector<std::uint8_t> SMIMEA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->usage, store);
    codec::pack(this->selector, store);
    codec::pack(this->matchingtype, store);
    codec::pack(this->certificate, store, false);
    return std::move(store);
}

std::size_t SMIMEA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->usage, data, size);
    temp += codec::unpack(this->selector, data + temp, size - temp);
    temp += codec::unpack(this->matchingtype, data + temp, size - temp);
    temp += codec::unpack(this->certificate, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// HIP
//std::vector<std::uint8_t> HIP::data() const
//{
//    std::vector<std::uint8_t> store;
//    codec::pack(this->hitlength, store);
//    codec::pack(this->pkalgorithm, store);
//    codec::pack(this->pklength, store);
//    codec::pack(this->hit, store, false);
//    codec::pack(this->publickey, store, false);
//    codec::pack(this->rendezvousservers, store, false);
//    return std::move(store);
//}
//
//std::size_t HIP::setData(const std::uint8_t *data, std::size_t size)
//{
//    std::size_t temp = codec::unpack(this->hitlength, data, size);
//    temp += codec::unpack(this->pkalgorithm, data + temp, size - temp);
//    temp += codec::unpack(this->pklength, data + temp, size - temp);
//    temp += codec::unpack(this->hit, data + temp, size - temp, false);
//    temp += codec::unpack(this->publickey, data + temp, size - temp, false);
//    temp += codec::unpack(this->rendezvousservers, data + temp, size - temp, false);
//    return temp;
//}


// -----------------------------------------------------------------------------
// NINFO
std::vector<std::uint8_t> NINFO::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->zs_data, store, false);
    return std::move(store);
}

std::size_t NINFO::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->zs_data, data, size, false);
}


// -----------------------------------------------------------------------------
// RKEY
std::vector<std::uint8_t> RKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->protocol, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->publickey, store, false);
    return std::move(store);
}

std::size_t RKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// TALINK
std::vector<std::uint8_t> TALINK::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->previousname, store, true);
    codec::pack(this->nextname, store, true);
    return std::move(store);
}

std::size_t TALINK::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->previousname, data, size, true);
    temp += codec::unpack(this->nextname, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// CDS
std::vector<std::uint8_t> CDS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->keytag, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->digesttype, store);
    codec::pack(this->digest, store, false);
    return std::move(store);
}

std::size_t CDS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// CDNSKEY
std::vector<std::uint8_t> CDNSKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->protocol, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->publickey, store, false);
    return std::move(store);
}

std::size_t CDNSKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
std::vector<std::uint8_t> OPENPGPKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->publickey, store, false);
    return std::move(store);
}

std::size_t OPENPGPKEY::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->publickey, data, size, false);
}


// -----------------------------------------------------------------------------
// CSYNC
std::vector<std::uint8_t> CSYNC::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->serial, store);
    codec::pack(this->flags, store);
    store.insert(store.cend(), this->typebitmap.cbegin(), this->typebitmap.cend());
    return std::move(store);
}

std::size_t CSYNC::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->serial, data, size);
    temp += codec::unpack(this->flags, data + temp, size - temp);

    this->typebitmap.clear();
    this->typebitmap.insert(this->typebitmap.cbegin(), data + temp, data + size);

    return size;
}


// -----------------------------------------------------------------------------
// SPF
std::vector<std::uint8_t> SPF::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->txt, store, false);
    return std::move(store);
}

std::size_t SPF::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->txt, data, size, false);
}


// -----------------------------------------------------------------------------
// NID
std::vector<std::uint8_t> NID::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->nodeid, store);
    return std::move(store);
}

std::size_t NID::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->nodeid, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// L32
std::vector<std::uint8_t> L32::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->locator32, store);
    return std::move(store);
}

std::size_t L32::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->locator32, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// L64
std::vector<std::uint8_t> L64::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->locator64, store);
    return std::move(store);
}

std::size_t L64::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->locator64, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// LP
std::vector<std::uint8_t> LP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->fqdn, store, true);
    return std::move(store);
}

std::size_t LP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->fqdn, data + temp, size - temp, true);
    return temp;
}


// -----------------------------------------------------------------------------
// EUI48
std::vector<std::uint8_t> EUI48::data() const
{
    std::vector<std::uint8_t> store;
    store.insert(store.cbegin(), this->address.cbegin(), this->address.cend());
    return std::move(store);
}

std::size_t EUI48::setData(const std::uint8_t *data, std::size_t size)
{
    if (size < 6)
        throw error_size(str::format("codec unpack EUI48 size is not enough, require 6 bytes"));

    std::copy(data, data + 6, this->address.begin());
    return 6;
}


// -----------------------------------------------------------------------------
// EUI64
std::vector<std::uint8_t> EUI64::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->address, store);
    return std::move(store);
}

std::size_t EUI64::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->address, data, size);
}


// -----------------------------------------------------------------------------
// TKEY
std::vector<std::uint8_t> TKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->algorithm, store, true);
    codec::pack(this->inception, store);
    codec::pack(this->expiration, store);
    codec::pack(this->mode, store);
    codec::pack(this->error, store);
    codec::pack(this->keysize, store);
    store.insert(store.cend(), this->key.cbegin(), this->key.cbegin() + this->keysize);
    codec::pack(this->otherlen, store);
    store.insert(store.cend(), this->otherdata.cbegin(), this->otherdata.cbegin() + this->otherlen);
    return std::move(store);
}

std::size_t TKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->algorithm, data, size, true);
    temp += codec::unpack(this->inception, data + temp, size - temp);
    temp += codec::unpack(this->expiration, data + temp, size - temp);
    temp += codec::unpack(this->mode, data + temp, size - temp);
    temp += codec::unpack(this->error, data + temp, size - temp);
    temp += codec::unpack(this->keysize, data + temp, size - temp);

    this->key.clear();
    this->key.insert(this->key.cbegin(), data + temp, data + temp + this->keysize);

    temp += this->keysize;
    temp += codec::unpack(this->otherlen, data + temp, size - temp);

    this->otherdata.clear();
    this->otherdata.insert(this->otherdata.cbegin(), data + temp, data + temp + this->otherlen);

    temp += this->otherlen;
    return temp;
}


// -----------------------------------------------------------------------------
// TSIG
std::vector<std::uint8_t> TSIG::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->algorithm, store, true);
    std::copy(this->timesigned.cbegin(), this->timesigned.cend(), store.end());
    codec::pack(this->fudge, store);
    codec::pack(this->macsize, store);
    store.insert(store.cbegin(), this->mac.cbegin(), this->mac.cbegin() + this->macsize);
    codec::pack(this->originalid, store);
    codec::pack(this->error, store);
    codec::pack(this->otherlen, store);
    store.insert(store.cbegin(), this->otherdata.cbegin(), this->otherdata.cbegin() + this->otherlen);
    return std::move(store);
}

std::size_t TSIG::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->algorithm, data, size, true);

    auto need = this->timesigned.size();

    if (size < need)
        throw error_size(str::format("codec unpack TSIG size is not enough, require %d bytes", need));

    std::copy(data + temp, data + temp + need, this->timesigned.begin());

    temp += need;
    temp += codec::unpack(this->fudge, data + temp, size - temp);
    temp += codec::unpack(this->macsize, data + temp, size - temp);

    this->mac.clear();
    this->mac.insert(this->mac.cbegin(), data + temp, data + temp + this->macsize);

    temp += this->macsize;
    temp += codec::unpack(this->originalid, data + temp, size - temp);
    temp += codec::unpack(this->error, data + temp, size - temp);
    temp += codec::unpack(this->otherlen, data + temp, size - temp);

    this->otherdata.clear();
    this->otherdata.insert(this->otherdata.cbegin(), data + temp, data + temp + this->otherlen);

    temp += this->otherlen;
    return temp;
}


// -----------------------------------------------------------------------------
// URI
std::vector<std::uint8_t> URI::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->priority, store);
    codec::pack(this->weight, store);
    codec::pack(this->target, store, false);
    return std::move(store);
}

std::size_t URI::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->priority, data, size);
    temp += codec::unpack(this->weight, data + temp, size - temp);
    temp += codec::unpack(this->target, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// CAA
std::vector<std::uint8_t> CAA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->tag, store, false);
    codec::pack(this->value, store, false);
    return std::move(store);
}

std::size_t CAA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->tag, data + temp, size - temp, false);
    temp += codec::unpack(this->value, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// TA
std::vector<std::uint8_t> TA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->keytag, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->digesttype, store);
    codec::pack(this->digest, store, false);
    return std::move(store);
}

std::size_t TA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, data + temp, size - temp, false);
    return temp;
}


// -----------------------------------------------------------------------------
// DLV
std::vector<std::uint8_t> DLV::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->keytag, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->digesttype, store);
    codec::pack(this->digest, store, false);
    return std::move(store);
}

std::size_t DLV::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, data + temp, size - temp, false);
    return temp;
}