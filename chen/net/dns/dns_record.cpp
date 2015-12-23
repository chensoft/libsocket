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


// -----------------------------------------------------------------------------
// EID
std::vector<std::uint8_t> EID::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->endpoint, false, store);
    return std::move(store);
}

std::size_t EID::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->endpoint, false, data, size);
}


// -----------------------------------------------------------------------------
// NIMLOC
std::vector<std::uint8_t> NIMLOC::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->locator, false, store);
    return std::move(store);
}

std::size_t NIMLOC::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->locator, false, data, size);
}


// -----------------------------------------------------------------------------
// SRV
std::vector<std::uint8_t> SRV::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->priority, store);
    codec::pack(this->weight, store);
    codec::pack(this->port, store);
    codec::pack(this->target, true, store);
    return std::move(store);
}

std::size_t SRV::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->priority, data, size);
    temp += codec::unpack(this->weight, data + temp, size - temp);
    temp += codec::unpack(this->port, data + temp, size - temp);
    temp += codec::unpack(this->target, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// ATMA
std::vector<std::uint8_t> ATMA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->format, store);
    codec::pack(this->address, false, store);
    return std::move(store);
}

std::size_t ATMA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->format, data, size);
    temp += codec::unpack(this->address, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// NAPTR
std::vector<std::uint8_t> NAPTR::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->order, store);
    codec::pack(this->preference, store);
    codec::pack(this->flags, false, store);
    codec::pack(this->services, false, store);
    codec::pack(this->regexp, false, store);
    codec::pack(this->replacement, true, store);
    return std::move(store);
}

std::size_t NAPTR::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->order, data, size);
    temp += codec::unpack(this->preference, data + temp, size - temp);
    temp += codec::unpack(this->flags, false, data + temp, size - temp);
    temp += codec::unpack(this->services, false, data + temp, size - temp);
    temp += codec::unpack(this->regexp, false, data + temp, size - temp);
    temp += codec::unpack(this->replacement, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// KX
std::vector<std::uint8_t> KX::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->preference, store);
    codec::pack(this->exchanger, true, store);
    return std::move(store);
}

std::size_t KX::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->exchanger, true, data + temp, size - temp);
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
    codec::pack(this->certificate, false, store);
    return std::move(store);
}

std::size_t CERT::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->type, data, size);
    temp += codec::unpack(this->keytag, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->certificate, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// DNAME
std::vector<std::uint8_t> DNAME::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->target, true, store);
    return std::move(store);
}

std::size_t DNAME::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->target, true, data, size);
}


// -----------------------------------------------------------------------------
// DS
std::vector<std::uint8_t> DS::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->keytag, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->digesttype, store);
    codec::pack(this->digest, false, store);
    return std::move(store);
}

std::size_t DS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// SSHFP
std::vector<std::uint8_t> SSHFP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->algorithm, store);
    codec::pack(this->fptype, store);
    codec::pack(this->fingerprint, false, store);
    return std::move(store);
}

std::size_t SSHFP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->algorithm, data, size);
    temp += codec::unpack(this->fptype, data + temp, size - temp);
    temp += codec::unpack(this->fingerprint, false, data + temp, size - temp);
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
            codec::pack(".", false, store);
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
            codec::pack(domain, true, store);
        }
            break;
    }

    codec::pack(this->publickey, false, store);
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

            std::copy(data + temp, data + temp + 4, this->gateway.begin());

            temp += 4;
            break;

        case GatewayType::IPv6:
            if (size - temp < 16)
                throw error_size(str::format("codec unpack IPSECKEY size is not enough, require 16 bytes"));

            std::copy(data + temp, data + temp + 16, this->gateway.begin());

            temp += 16;
            break;

        case GatewayType::Domain:
        {
            std::string domain;
            temp += codec::unpack(domain, true, data, size);

            std::copy(domain.cbegin(), domain.cend(), this->gateway.begin());
        }
            break;
    }

    temp += codec::unpack(this->publickey, false, data + temp, size - temp);
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
    codec::pack(this->signer, true, store);
    codec::pack(this->signature, false, store);
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
    temp += codec::unpack(this->signer, true, data + temp, size - temp);
    temp += codec::unpack(this->signature, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// NSEC
std::vector<std::uint8_t> NSEC::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->nextdomain, true, store);
    std::copy(this->typebitmap.cbegin(), this->typebitmap.cend(), store.end());
    return std::move(store);
}

std::size_t NSEC::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->nextdomain, true, data, size);

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
    codec::pack(this->publickey, false, store);
    return std::move(store);
}

std::size_t DNSKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// DHCID
std::vector<std::uint8_t> DHCID::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->digest, false, store);
    return std::move(store);
}

std::size_t DHCID::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->digest, false, data, size);
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
    codec::pack(this->salt, false, store);
    codec::pack(this->hashlength, store);
    codec::pack(this->nextowner, false, store);
    std::copy(this->typebitmap.cbegin(), this->typebitmap.cend(), store.end());
    return std::move(store);
}

std::size_t NSEC3::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->hash, data, size);
    temp += codec::unpack(this->flags, data + temp, size - temp);
    temp += codec::unpack(this->iterations, data + temp, size - temp);
    temp += codec::unpack(this->saltLength, data + temp, size - temp);
    temp += codec::unpack(this->salt, false, data + temp, size - temp);
    temp += codec::unpack(this->hashlength, data + temp, size - temp);
    temp += codec::unpack(this->nextowner, false, data + temp, size - temp);

    this->typebitmap.clear();
    std::copy(data + temp, data + size, this->typebitmap.begin());

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
    codec::pack(this->salt, false, store);
    return std::move(store);
}

std::size_t NSEC3PARAM::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->hash, data, size);
    temp += codec::unpack(this->flags, data + temp, size - temp);
    temp += codec::unpack(this->iterations, data + temp, size - temp);
    temp += codec::unpack(this->saltLength, data + temp, size - temp);
    temp += codec::unpack(this->salt, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// TLSA
std::vector<std::uint8_t> TLSA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->usage, store);
    codec::pack(this->selector, store);
    codec::pack(this->matchingtype, store);
    codec::pack(this->certificate, false, store);
    return std::move(store);
}

std::size_t TLSA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->usage, data, size);
    temp += codec::unpack(this->selector, data + temp, size - temp);
    temp += codec::unpack(this->matchingtype, data + temp, size - temp);
    temp += codec::unpack(this->certificate, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// HIP
std::vector<std::uint8_t> HIP::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->hitlength, store);
    codec::pack(this->pkalgorithm, store);
    codec::pack(this->pklength, store);
    codec::pack(this->hit, false, store);
    codec::pack(this->publickey, false, store);
    codec::pack(this->rendezvousservers, false, store);
    return std::move(store);
}

std::size_t HIP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->hitlength, data, size);
    temp += codec::unpack(this->pkalgorithm, data + temp, size - temp);
    temp += codec::unpack(this->pklength, data + temp, size - temp);
    temp += codec::unpack(this->hit, false, data + temp, size - temp);
    temp += codec::unpack(this->publickey, false, data + temp, size - temp);
    temp += codec::unpack(this->rendezvousservers, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// NINFO
std::vector<std::uint8_t> NINFO::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->zs_data, false, store);
    return std::move(store);
}

std::size_t NINFO::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->zs_data, false, data, size);
}


// -----------------------------------------------------------------------------
// RKEY
std::vector<std::uint8_t> RKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->protocol, store);
    codec::pack(this->algorithm, store);
    codec::pack(this->publickey, false, store);
    return std::move(store);
}

std::size_t RKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// TALINK
std::vector<std::uint8_t> TALINK::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->previous, false, store);
    codec::pack(this->next, false, store);
    return std::move(store);
}

std::size_t TALINK::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->previous, false, data, size);
    temp += codec::unpack(this->next, false, data + temp, size - temp);
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
    codec::pack(this->digest, false, store);
    return std::move(store);
}

std::size_t CDS::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, false, data + temp, size - temp);
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
    codec::pack(this->publickey, false, store);
    return std::move(store);
}

std::size_t CDNSKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->protocol, data + temp, size - temp);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->publickey, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
std::vector<std::uint8_t> OPENPGPKEY::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->publickey, false, store);
    return std::move(store);
}

std::size_t OPENPGPKEY::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->publickey, false, data, size);
}


// -----------------------------------------------------------------------------
// SPF
std::vector<std::uint8_t> SPF::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->txt, false, store);
    return std::move(store);
}

std::size_t SPF::setData(const std::uint8_t *data, std::size_t size)
{
    return codec::unpack(this->txt, false, data, size);
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
    codec::pack(this->fqdn, true, store);
    return std::move(store);
}

std::size_t LP::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->preference, data, size);
    temp += codec::unpack(this->fqdn, true, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// EUI48
std::vector<std::uint8_t> EUI48::data() const
{
    std::vector<std::uint8_t> store;
    std::copy(this->address.cbegin(), this->address.cend(), store.begin());
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
    codec::pack(this->algorithm, true, store);
    codec::pack(this->inception, store);
    codec::pack(this->expiration, store);
    codec::pack(this->mode, store);
    codec::pack(this->error, store);
    codec::pack(this->keysize, store);
    codec::pack(this->key, false, store);
    codec::pack(this->otherlen, store);
    codec::pack(this->otherdata, false, store);
    return std::move(store);
}

std::size_t TKEY::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->algorithm, true, data, size);
    temp += codec::unpack(this->inception, data + temp, size - temp);
    temp += codec::unpack(this->expiration, data + temp, size - temp);
    temp += codec::unpack(this->mode, data + temp, size - temp);
    temp += codec::unpack(this->error, data + temp, size - temp);
    temp += codec::unpack(this->keysize, data + temp, size - temp);
    temp += codec::unpack(this->key, false, data + temp, size - temp);
    temp += codec::unpack(this->otherlen, data + temp, size - temp);
    temp += codec::unpack(this->otherdata, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// TSIG
std::vector<std::uint8_t> TSIG::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->algorithm, true, store);
    std::copy(this->timesigned.cbegin(), this->timesigned.cend(), store.end());
    codec::pack(this->fudge, store);
    codec::pack(this->macsize, store);
    codec::pack(this->mac, false, store);
    codec::pack(this->originalid, store);
    codec::pack(this->error, store);
    codec::pack(this->otherlen, store);
    codec::pack(this->otherdata, false, store);
    return std::move(store);
}

std::size_t TSIG::setData(const std::uint8_t *data, std::size_t size)
{
    std::string algorithm;
    std::size_t temp = codec::unpack(algorithm, true, data, size);

    auto need = this->timesigned.size();

    if (size < need)
        throw error_size(str::format("codec unpack TSIG size is not enough, require %d bytes", need));

    std::copy(data + temp, data + temp + need, this->timesigned.begin());

    this->algorithm = algorithm;

    temp += need;
    temp += codec::unpack(this->fudge, data + temp, size - temp);
    temp += codec::unpack(this->macsize, data + temp, size - temp);
    temp += codec::unpack(this->mac, false, data + temp, size - temp);
    temp += codec::unpack(this->originalid, data + temp, size - temp);
    temp += codec::unpack(this->error, data + temp, size - temp);
    temp += codec::unpack(this->otherlen, data + temp, size - temp);
    temp += codec::unpack(this->otherdata, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// URI
std::vector<std::uint8_t> URI::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->priority, store);
    codec::pack(this->weight, store);
    codec::pack(this->target, false, store);
    return std::move(store);
}

std::size_t URI::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->priority, data, size);
    temp += codec::unpack(this->weight, data + temp, size - temp);
    temp += codec::unpack(this->target, false, data + temp, size - temp);
    return temp;
}


// -----------------------------------------------------------------------------
// CAA
std::vector<std::uint8_t> CAA::data() const
{
    std::vector<std::uint8_t> store;
    codec::pack(this->flags, store);
    codec::pack(this->tag, false, store);
    codec::pack(this->value, false, store);
    return std::move(store);
}

std::size_t CAA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->flags, data, size);
    temp += codec::unpack(this->tag, false, data + temp, size - temp);
    temp += codec::unpack(this->value, false, data + temp, size - temp);
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
    codec::pack(this->digest, false, store);
    return std::move(store);
}

std::size_t TA::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, false, data + temp, size - temp);
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
    codec::pack(this->digest, false, store);
    return std::move(store);
}

std::size_t DLV::setData(const std::uint8_t *data, std::size_t size)
{
    std::size_t temp = codec::unpack(this->keytag, data, size);
    temp += codec::unpack(this->algorithm, data + temp, size - temp);
    temp += codec::unpack(this->digesttype, data + temp, size - temp);
    temp += codec::unpack(this->digest, false, data + temp, size - temp);
    return temp;
}