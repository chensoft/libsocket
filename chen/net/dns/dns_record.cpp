/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_record.hpp"
#include "dns_error.hpp"
#include "dns_codec.hpp"
#include <chen/tool/str.hpp>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR
RR::~RR()
{

}

void RR::decode(chen::dns::decoder &decoder)
{
    decoder.unpack(this->rdlength);
}

std::size_t RR::remain(chen::dns::decoder &decoder, std::size_t origin) const
{
    std::size_t used = decoder.cursor() - origin;
    if (this->rdlength < used)
        throw error_size("codec rdata is overflow");

    return this->rdlength - used;
}


// -----------------------------------------------------------------------------
// RAW
void Raw::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->rdata, this->rdata.size());
}

void Raw::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->rdata, this->rdlength);
}


// -----------------------------------------------------------------------------
// A
void A::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void A::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// NS
void NS::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->nsdname, true);
}

void NS::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->nsdname, true);
}


// -----------------------------------------------------------------------------
// MD
void MD::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->madname, true);
}

void MD::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->madname, true);
}


// -----------------------------------------------------------------------------
// MF
void MF::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->madname, true);
}

void MF::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->madname, true);
}


// -----------------------------------------------------------------------------
// CNAME
void CNAME::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->cname, true);
}

void CNAME::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->cname, true);
}


// -----------------------------------------------------------------------------
// SOA
void SOA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->mname, true);
    encoder.pack(this->rname, true);
    encoder.pack(this->serial);
    encoder.pack(this->refresh);
    encoder.pack(this->retry);
    encoder.pack(this->expire);
    encoder.pack(this->minimum);
}

void SOA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->mname, true);
    decoder.unpack(this->rname, true);
    decoder.unpack(this->serial);
    decoder.unpack(this->refresh);
    decoder.unpack(this->retry);
    decoder.unpack(this->expire);
    decoder.unpack(this->minimum);
}


// -----------------------------------------------------------------------------
// MB
void MB::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->madname, true);
}

void MB::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->madname, true);
}


// -----------------------------------------------------------------------------
// MG
void MG::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->mgmname, true);
}

void MG::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->mgmname, true);
}


// -----------------------------------------------------------------------------
// MR
void MR::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->newname, true);
}

void MR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->newname, true);
}


// -----------------------------------------------------------------------------
// RNULL
void RNULL::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->anything, this->anything.size());
}

void RNULL::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->anything, this->rdlength);
}


// -----------------------------------------------------------------------------
// WKS
void WKS::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
    encoder.pack(this->protocol);
    encoder.pack(this->bitmap, this->bitmap.size());
}

void WKS::decode(chen::dns::decoder &decoder)
{
    auto origin = decoder.cursor();

    RR::decode(decoder);

    decoder.unpack(this->address);
    decoder.unpack(this->protocol);

    this->bitmap.clear();
    decoder.unpack(this->bitmap, this->remain(decoder, origin));
}


// -----------------------------------------------------------------------------
// PTR
void PTR::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->ptrdname, true);
}

void PTR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->ptrdname, true);
}


// -----------------------------------------------------------------------------
// HINFO
void HINFO::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->cpu, false);
    encoder.pack(this->os, false);
}

void HINFO::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->cpu, false);
    decoder.unpack(this->os, false);
}


// -----------------------------------------------------------------------------
// MINFO
void MINFO::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->rmailbx, true);
    encoder.pack(this->emailbx, true);
}

void MINFO::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->rmailbx, true);
    decoder.unpack(this->emailbx, true);
}


// -----------------------------------------------------------------------------
// MX
void MX::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchange, true);
}

void MX::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->exchange, true);
}


// -----------------------------------------------------------------------------
// TXT
void TXT::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->txt_data, false);
}

void TXT::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->txt_data, false);
}


// -----------------------------------------------------------------------------
// RP
void RP::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->mbox_dname, true);
    encoder.pack(this->txt_dname, true);
}

void RP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->mbox_dname, true);
    decoder.unpack(this->txt_dname, true);
}


// -----------------------------------------------------------------------------
// AFSDB
void AFSDB::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->subtype);
    encoder.pack(this->hostname, true);
}

void AFSDB::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->subtype);
    decoder.unpack(this->hostname, true);
}


// -----------------------------------------------------------------------------
// X25
void X25::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->psdn_address, false);
}

void X25::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->psdn_address, false);
}


// -----------------------------------------------------------------------------
// ISDN
void ISDN::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->isdn_address, false);
    encoder.pack(this->sa, false);
}

void ISDN::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->isdn_address, false);
    decoder.unpack(this->sa, false);
}


// -----------------------------------------------------------------------------
// RT
void RT::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->intermediate_host, true);
}

void RT::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->intermediate_host, true);
}


// -----------------------------------------------------------------------------
// NSAP
void NSAP::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->nsap, false);
}

void NSAP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->nsap, false);
}


// -----------------------------------------------------------------------------
// NSAPPTR
void NSAPPTR::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->owner, true);
}

void NSAPPTR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->owner, true);
}


// -----------------------------------------------------------------------------
// SIG
void SIG::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->type_covered);
    encoder.pack(this->algorithm);
    encoder.pack(this->labels);
    encoder.pack(this->original);
    encoder.pack(this->expiration);
    encoder.pack(this->inception);
    encoder.pack(this->key_tag);
    encoder.pack(this->signer, true);
    encoder.pack(this->signature, false);
}

void SIG::decode(chen::dns::decoder &decoder)
{
    decoder.unpack(this->type_covered);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->labels);
    decoder.unpack(this->original);
    decoder.unpack(this->expiration);
    decoder.unpack(this->inception);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->signer, true);
    decoder.unpack(this->signature, false);
}


// -----------------------------------------------------------------------------
// KEY
void KEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, false);
}

void KEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// PX
void PX::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->map822, true);
    encoder.pack(this->mapx400, true);
}

void PX::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->map822, true);
    decoder.unpack(this->mapx400, true);
}


// -----------------------------------------------------------------------------
// GPOS
void GPOS::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->longitude, false);
    encoder.pack(this->latitude, false);
    encoder.pack(this->altitude, false);
}

void GPOS::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->longitude, false);
    decoder.unpack(this->latitude, false);
    decoder.unpack(this->altitude, false);
}


// -----------------------------------------------------------------------------
// AAAA
void AAAA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void AAAA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// LOC
void LOC::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->size);
    encoder.pack(this->horiz_pre);
    encoder.pack(this->vert_pre);
    encoder.pack(this->longitude);
    encoder.pack(this->latitude);
    encoder.pack(this->altitude);
}

void LOC::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->version);
    decoder.unpack(this->size);
    decoder.unpack(this->horiz_pre);
    decoder.unpack(this->vert_pre);
    decoder.unpack(this->longitude);
    decoder.unpack(this->latitude);
    decoder.unpack(this->altitude);
}


// -----------------------------------------------------------------------------
// NXT
void NXT::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, true);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void NXT::decode(chen::dns::decoder &decoder)
{
    auto origin = decoder.cursor();

    RR::decode(decoder);
    decoder.unpack(this->next_domain, true);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(decoder, origin));
}


// -----------------------------------------------------------------------------
// EID
void EID::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->endpoint, false);
}

void EID::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->endpoint, false);
}


// -----------------------------------------------------------------------------
// NIMLOC
void NIMLOC::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->locator, false);
}

void NIMLOC::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->locator, false);
}


// -----------------------------------------------------------------------------
// SRV
void SRV::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->port);
    encoder.pack(this->target, true);
}

void SRV::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->port);
    decoder.unpack(this->target, true);
}


// -----------------------------------------------------------------------------
// ATMA
void ATMA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->format);
    encoder.pack(this->address, false);
}

void ATMA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->format);
    decoder.unpack(this->address, false);
}


// -----------------------------------------------------------------------------
// NAPTR
void NAPTR::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->order);
    encoder.pack(this->preference);
    encoder.pack(this->flags, false);
    encoder.pack(this->services, false);
    encoder.pack(this->regexp, false);
    encoder.pack(this->replacement, true);
}

void NAPTR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->order);
    decoder.unpack(this->preference);
    decoder.unpack(this->flags, false);
    decoder.unpack(this->services, false);
    decoder.unpack(this->regexp, false);
    decoder.unpack(this->replacement, true);
}


// -----------------------------------------------------------------------------
// KX
void KX::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchanger, true);
}

void KX::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->exchanger, true);
}


// -----------------------------------------------------------------------------
// CERT
void CERT::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->type);
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->certificate, false);
}

void CERT::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->type);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->certificate, false);
}


// -----------------------------------------------------------------------------
// DNAME
void DNAME::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->target, true);
}

void DNAME::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->target, true);
}


// -----------------------------------------------------------------------------
// SINK
void SINK::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->coding);
    encoder.pack(this->subcoding);
    encoder.pack(this->sdata, this->sdata.size());
}

void SINK::decode(chen::dns::decoder &decoder)
{
    auto origin = decoder.cursor();

    RR::decode(decoder);
    decoder.unpack(this->coding);
    decoder.unpack(this->subcoding);

    this->sdata.clear();
    decoder.unpack(this->sdata, this->remain(decoder, origin));
}


// -----------------------------------------------------------------------------
// DS
void DS::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, false);
}

void DS::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, false);
}


// -----------------------------------------------------------------------------
// SSHFP
void SSHFP::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->algorithm);
    encoder.pack(this->fptype);
    encoder.pack(this->fingerprint, false);
}

void SSHFP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->fptype);
    decoder.unpack(this->fingerprint, false);
}


// -----------------------------------------------------------------------------
// IPSECKEY
void IPSECKEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->precedence);
    encoder.pack(this->gateway_type);
    encoder.pack(this->algorithm);

    switch (static_cast<GatewayType>(this->gateway_type))
    {
        case GatewayType::None:
            encoder.pack(this->gateway, 1);
            break;

        case GatewayType::IPv4:
            encoder.pack(this->gateway, 4);
            break;

        case GatewayType::IPv6:
            encoder.pack(this->gateway, 16);
            break;

        case GatewayType::Domain:
        {
            std::string domain(this->gateway.begin(), this->gateway.end());
            encoder.pack(domain, true);
        }
            break;
    }

    encoder.pack(this->publickey, false);
}

void IPSECKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->precedence);
    decoder.unpack(this->gateway_type);
    decoder.unpack(this->algorithm);

    this->gateway.clear();

    switch (static_cast<GatewayType>(this->gateway_type))
    {
        case GatewayType::None:
            decoder.unpack(this->gateway, 1);
            break;

        case GatewayType::IPv4:
            decoder.unpack(this->gateway, 4);
            break;

        case GatewayType::IPv6:
            decoder.unpack(this->gateway, 16);
            break;

        case GatewayType::Domain:
        {
            std::string domain;
            decoder.unpack(domain, true);

            this->gateway.insert(this->gateway.begin(), domain.begin(), domain.end());
        }
            break;
    }

    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// RRSIG
void RRSIG::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->type_covered);
    encoder.pack(this->algorithm);
    encoder.pack(this->labels);
    encoder.pack(this->original);
    encoder.pack(this->expiration);
    encoder.pack(this->inception);
    encoder.pack(this->key_tag);
    encoder.pack(this->signer, true);
    encoder.pack(this->signature, false);
}

void RRSIG::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->type_covered);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->labels);
    decoder.unpack(this->original);
    decoder.unpack(this->expiration);
    decoder.unpack(this->inception);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->signer, true);
    decoder.unpack(this->signature, false);
}


// -----------------------------------------------------------------------------
// NSEC
void NSEC::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, true);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void NSEC::decode(chen::dns::decoder &decoder)
{
    auto origin = decoder.cursor();

    RR::decode(decoder);
    decoder.unpack(this->next_domain, true);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(decoder, origin));
}


// -----------------------------------------------------------------------------
// DNSKEY
void DNSKEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, false);
}

void DNSKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// DHCID
void DHCID::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->digest, false);
}

void DHCID::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->digest, false);
}


// -----------------------------------------------------------------------------
// NSEC3
void NSEC3::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->hash);
    encoder.pack(this->flags);
    encoder.pack(this->iterations);
    encoder.pack(this->salt_length);
    encoder.pack(this->salt, this->salt_length);
    encoder.pack(this->hash_length);
    encoder.pack(this->next_owner, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void NSEC3::decode(chen::dns::decoder &decoder)
{
    auto origin = decoder.cursor();

    RR::decode(decoder);
    decoder.unpack(this->hash);
    decoder.unpack(this->flags);
    decoder.unpack(this->iterations);
    decoder.unpack(this->salt_length);
    decoder.unpack(this->salt, this->salt_length);
    decoder.unpack(this->hash_length);
    decoder.unpack(this->next_owner, false);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(decoder, origin));
}


// -----------------------------------------------------------------------------
// NSEC3PARAM
void NSEC3PARAM::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->hash);
    encoder.pack(this->flags);
    encoder.pack(this->iterations);
    encoder.pack(this->salt_length);
    encoder.pack(this->salt, this->salt_length);
}

void NSEC3PARAM::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->hash);
    decoder.unpack(this->flags);
    decoder.unpack(this->iterations);
    decoder.unpack(this->salt_length);
    decoder.unpack(this->salt, this->salt_length);
}


// -----------------------------------------------------------------------------
// TLSA
void TLSA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, false);
}

void TLSA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, false);
}


// -----------------------------------------------------------------------------
// SMIMEA
void SMIMEA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, false);
}

void SMIMEA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, false);
}


// -----------------------------------------------------------------------------
// HIP
void HIP::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->hit_length);
    encoder.pack(this->pk_algorithm);
    encoder.pack(this->pk_length);
    encoder.pack(this->hit, false);
    encoder.pack(this->publickey, false);
    encoder.pack(this->rendezvous_servers, false);
}

void HIP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->hit_length);
    decoder.unpack(this->pk_algorithm);
    decoder.unpack(this->pk_length);
    decoder.unpack(this->hit, false);
    decoder.unpack(this->publickey, false);
    decoder.unpack(this->rendezvous_servers, false);
}


// -----------------------------------------------------------------------------
// NINFO
void NINFO::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->zs_data, false);
}

void NINFO::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->zs_data, false);
}


// -----------------------------------------------------------------------------
// RKEY
void RKEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, false);
}

void RKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// TALINK
void TALINK::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->previous_name, true);
    encoder.pack(this->next_name, true);
}

void TALINK::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->previous_name, true);
    decoder.unpack(this->next_name, true);
}


// -----------------------------------------------------------------------------
// CDS
void CDS::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, false);
}

void CDS::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, false);
}


// -----------------------------------------------------------------------------
// CDNSKEY
void CDNSKEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, false);
}

void CDNSKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
void OPENPGPKEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->publickey, false);
}

void OPENPGPKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// CSYNC
void CSYNC::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->serial);
    encoder.pack(this->flags);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void CSYNC::decode(chen::dns::decoder &decoder)
{
    auto origin = decoder.cursor();

    RR::decode(decoder);
    decoder.unpack(this->serial);
    decoder.unpack(this->flags);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(decoder, origin));
}


// -----------------------------------------------------------------------------
// SPF
void SPF::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->txt, false);
}

void SPF::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->txt, false);
}


// -----------------------------------------------------------------------------
// NID
void NID::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->node_id);
}

void NID::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->node_id);
}


// -----------------------------------------------------------------------------
// L32
void L32::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator32);
}

void L32::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->locator32);
}


// -----------------------------------------------------------------------------
// L64
void L64::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator64);
}

void L64::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->locator64);
}


// -----------------------------------------------------------------------------
// LP
void LP::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->fqdn, true);
}

void LP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->fqdn, true);
}


// -----------------------------------------------------------------------------
// EUI48
void EUI48::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void EUI48::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// EUI64
void EUI64::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void EUI64::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// TKEY
void TKEY::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->algorithm, true);
    encoder.pack(this->inception);
    encoder.pack(this->expiration);
    encoder.pack(this->mode);
    encoder.pack(this->error);
    encoder.pack(this->key_size);
    encoder.pack(this->key, this->key_size);
    encoder.pack(this->other_len);
    encoder.pack(this->other_data, this->other_len);
}

void TKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->algorithm, true);
    decoder.unpack(this->inception);
    decoder.unpack(this->expiration);
    decoder.unpack(this->mode);
    decoder.unpack(this->error);
    decoder.unpack(this->key_size);
    decoder.unpack(this->key, this->key_size);
    decoder.unpack(this->other_len);
    decoder.unpack(this->other_data, this->other_len);
}


// -----------------------------------------------------------------------------
// TSIG
void TSIG::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->algorithm, true);
    encoder.pack(this->time_signed);
    encoder.pack(this->fudge);
    encoder.pack(this->mac_size);
    encoder.pack(this->mac, this->mac_size);
    encoder.pack(this->original_id);
    encoder.pack(this->error);
    encoder.pack(this->other_len);
    encoder.pack(this->other_data, this->other_len);
}

void TSIG::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->algorithm, true);
    decoder.unpack(this->time_signed);
    decoder.unpack(this->fudge);
    decoder.unpack(this->mac_size);
    decoder.unpack(this->mac, this->mac_size);
    decoder.unpack(this->original_id);
    decoder.unpack(this->error);
    decoder.unpack(this->other_len);
    decoder.unpack(this->other_data, this->other_len);
}


// -----------------------------------------------------------------------------
// URI
void URI::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->target, false);
}

void URI::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->target, false);
}


// -----------------------------------------------------------------------------
// CAA
void CAA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->tag, false);
    encoder.pack(this->value, false);
}

void CAA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->flags);
    decoder.unpack(this->tag, false);
    decoder.unpack(this->value, false);
}


// -----------------------------------------------------------------------------
// TA
void TA::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, false);
}

void TA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, false);
}


// -----------------------------------------------------------------------------
// DLV
void DLV::encode(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, false);
}

void DLV::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, false);
}