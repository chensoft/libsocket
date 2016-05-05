/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "dns_record.hpp"
#include "dns_error.hpp"
#include "dns_codec.hpp"
#include "dns_table.hpp"
#include <chen/tool/str.hpp>
#include <limits>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR
RR::~RR()
{

}

std::vector<std::uint8_t> RR::encode() const
{
    std::vector<std::uint8_t> out;
    this->encode(out);
    return out;  // todo test performance with std::move
}

void RR::encode(std::vector<std::uint8_t> &out) const
{
    // common
    encoder::pack(this->name, true, out);
    encoder::pack(this->rrtype, out);
    encoder::pack(this->rrclass, out);
    encoder::pack(this->ttl, out);

    // rdlength, placeholder here
    encoder::pack(static_cast<std::uint16_t>(0), out);
}

void RR::decode(chen::dns::decoder &decoder)
{
    decoder.unpack(this->rdlength);
}

std::shared_ptr<chen::dns::RR> RR::decode(const std::vector<std::uint8_t> &data)
{
    chen::dns::decoder decoder;
    decoder.assign(data);  // todo avoid copy, don't store vector?

    // name
    std::string name;
    decoder.unpack(name, true);

    // rrtype
    chen::dns::RRType rrtype = chen::dns::RRType::None;
    decoder.unpack(rrtype);

    // rrclass
    chen::dns::RRClass rrclass = chen::dns::RRClass::IN;
    decoder.unpack(rrclass);

    // ttl
    std::int32_t ttl = 0;
    decoder.unpack(ttl);

    // build
    // todo test if table build return nullptr?
    std::shared_ptr<chen::dns::RR> record = table::build(rrtype);
    if (!record)
        record.reset(new chen::dns::Unknown);

    // rdlength && rdata
    record->decode(decoder);

    // set
    record->name    = name;
    record->rrtype  = rrtype;
    record->rrclass = rrclass;
    record->ttl     = ttl;

    return record;  // todo use move?
}

// helper
void RR::adjust(std::vector<std::uint8_t> &out, std::size_t val) const
{
    if (val > std::numeric_limits<std::uint16_t>::max())
        throw error_size("dns: codec pack rdata size is overflow");

    auto rdlength = static_cast<std::uint16_t>(val);

    out[out.size() - rdlength - 2] = static_cast<std::uint8_t>(rdlength >> 8 & 0xFF);
    out[out.size() - rdlength - 1] = static_cast<std::uint8_t>(rdlength & 0xFF);
}

std::size_t RR::remain(chen::dns::decoder &decoder, std::size_t origin) const
{
    std::size_t used = decoder.cursor() - origin;
    if (this->rdlength < used)
        throw error_size("dns: codec rdata is overflow");

    return this->rdlength - used;
}


// -----------------------------------------------------------------------------
// RAW
void Raw::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->rdata, this->rdata.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void Raw::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->rdata, this->rdlength);
}


// -----------------------------------------------------------------------------
// A
void A::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->address, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void A::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// NS
void NS::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->nsdname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NS::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->nsdname, true);
}


// -----------------------------------------------------------------------------
// MD
void MD::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->madname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MD::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->madname, true);
}


// -----------------------------------------------------------------------------
// MF
void MF::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->madname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MF::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->madname, true);
}


// -----------------------------------------------------------------------------
// CNAME
void CNAME::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->cname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void CNAME::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->cname, true);
}


// -----------------------------------------------------------------------------
// SOA
void SOA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->mname, true, out);
        encoder::pack(this->rname, true, out);
        encoder::pack(this->serial, out);
        encoder::pack(this->refresh, out);
        encoder::pack(this->retry, out);
        encoder::pack(this->expire, out);
        encoder::pack(this->minimum, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void MB::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->madname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MB::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->madname, true);
}


// -----------------------------------------------------------------------------
// MG
void MG::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->mgmname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MG::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->mgmname, true);
}


// -----------------------------------------------------------------------------
// MR
void MR::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->newname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->newname, true);
}


// -----------------------------------------------------------------------------
// NUL
void NUL::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->anything, this->anything.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NUL::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->anything, this->rdlength);
}


// -----------------------------------------------------------------------------
// WKS
void WKS::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->address, out);
        encoder::pack(this->protocol, out);
        encoder::pack(this->bitmap, this->bitmap.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void PTR::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->ptrdname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void PTR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->ptrdname, true);
}


// -----------------------------------------------------------------------------
// HINFO
void HINFO::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->cpu, false, out);
        encoder::pack(this->os, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void HINFO::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->cpu, false);
    decoder.unpack(this->os, false);
}


// -----------------------------------------------------------------------------
// MINFO
void MINFO::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->rmailbx, true, out);
        encoder::pack(this->emailbx, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MINFO::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->rmailbx, true);
    decoder.unpack(this->emailbx, true);
}


// -----------------------------------------------------------------------------
// MX
void MX::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->exchange, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void MX::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->exchange, true);
}


// -----------------------------------------------------------------------------
// TXT
void TXT::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->txt_data, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void TXT::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->txt_data, false);
}


// -----------------------------------------------------------------------------
// RP
void RP::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->mbox_dname, true, out);
        encoder::pack(this->txt_dname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void RP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->mbox_dname, true);
    decoder.unpack(this->txt_dname, true);
}


// -----------------------------------------------------------------------------
// AFSDB
void AFSDB::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->subtype, out);
        encoder::pack(this->hostname, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void AFSDB::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->subtype);
    decoder.unpack(this->hostname, true);
}


// -----------------------------------------------------------------------------
// X25
void X25::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->psdn_address, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void X25::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->psdn_address, false);
}


// -----------------------------------------------------------------------------
// ISDN
void ISDN::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->isdn_address, false, out);
        encoder::pack(this->sa, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void ISDN::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->isdn_address, false);
    decoder.unpack(this->sa, false);
}


// -----------------------------------------------------------------------------
// RT
void RT::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->intermediate_host, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void RT::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->intermediate_host, true);
}


// -----------------------------------------------------------------------------
// NSAP
void NSAP::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->nsap, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NSAP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->nsap, false);
}


// -----------------------------------------------------------------------------
// NSAPPTR
void NSAPPTR::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->owner, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NSAPPTR::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->owner, true);
}


// -----------------------------------------------------------------------------
// SIG
void SIG::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->type_covered, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->labels, out);
        encoder::pack(this->original, out);
        encoder::pack(this->expiration, out);
        encoder::pack(this->inception, out);
        encoder::pack(this->key_tag, out);
        encoder::pack(this->signer, true, out);
        encoder::pack(this->signature, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void KEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->flags, out);
        encoder::pack(this->protocol, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->publickey, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void PX::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->map822, true, out);
        encoder::pack(this->mapx400, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void GPOS::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->longitude, false, out);
        encoder::pack(this->latitude, false, out);
        encoder::pack(this->altitude, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void AAAA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->address, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void AAAA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// LOC
void LOC::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->version, out);
        encoder::pack(this->size, out);
        encoder::pack(this->horiz_pre, out);
        encoder::pack(this->vert_pre, out);
        encoder::pack(this->longitude, out);
        encoder::pack(this->latitude, out);
        encoder::pack(this->altitude, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void NXT::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->next_domain, true, out);
        encoder::pack(this->type_bitmap, this->type_bitmap.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void EID::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->endpoint, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void EID::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->endpoint, false);
}


// -----------------------------------------------------------------------------
// NIMLOC
void NIMLOC::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->locator, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NIMLOC::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->locator, false);
}


// -----------------------------------------------------------------------------
// SRV
void SRV::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->priority, out);
        encoder::pack(this->weight, out);
        encoder::pack(this->port, out);
        encoder::pack(this->target, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void ATMA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->format, out);
        encoder::pack(this->address, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void ATMA::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->format);
    decoder.unpack(this->address, false);
}


// -----------------------------------------------------------------------------
// NAPTR
void NAPTR::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->order, out);
        encoder::pack(this->preference, out);
        encoder::pack(this->flags, false, out);
        encoder::pack(this->services, false, out);
        encoder::pack(this->regexp, false, out);
        encoder::pack(this->replacement, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void KX::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->exchanger, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void KX::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->exchanger, true);
}


// -----------------------------------------------------------------------------
// CERT
void CERT::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->type, out);
        encoder::pack(this->key_tag, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->certificate, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void DNAME::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->target, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void DNAME::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->target, true);
}


// -----------------------------------------------------------------------------
// SINK
void SINK::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->coding, out);
        encoder::pack(this->subcoding, out);
        encoder::pack(this->sdata, this->sdata.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void DS::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->key_tag, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->digest_type, out);
        encoder::pack(this->digest, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void SSHFP::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->algorithm, out);
        encoder::pack(this->fptype, out);
        encoder::pack(this->fingerprint, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void IPSECKEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->precedence, out);
        encoder::pack(this->gateway_type, out);
        encoder::pack(this->algorithm, out);

        switch (static_cast<GatewayType>(this->gateway_type))
        {
            case GatewayType::None:
                encoder::pack(this->gateway, 1, out);
                break;

            case GatewayType::IPv4:
                encoder::pack(this->gateway, 4, out);
                break;

            case GatewayType::IPv6:
                encoder::pack(this->gateway, 16, out);
                break;

            case GatewayType::Domain:
            {
                std::string domain(this->gateway.begin(), this->gateway.end());
                encoder::pack(domain, true, out);
            }
                break;
        }

        encoder::pack(this->publickey, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void RRSIG::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->type_covered, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->labels, out);
        encoder::pack(this->original, out);
        encoder::pack(this->expiration, out);
        encoder::pack(this->inception, out);
        encoder::pack(this->key_tag, out);
        encoder::pack(this->signer, true, out);
        encoder::pack(this->signature, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void NSEC::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->next_domain, true, out);
        encoder::pack(this->type_bitmap, this->type_bitmap.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void DNSKEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->flags, out);
        encoder::pack(this->protocol, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->publickey, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void DHCID::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->digest, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void DHCID::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->digest, false);
}


// -----------------------------------------------------------------------------
// NSEC3
void NSEC3::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->hash, out);
        encoder::pack(this->flags, out);
        encoder::pack(this->iterations, out);
        encoder::pack(this->salt_length, out);
        encoder::pack(this->salt, this->salt_length, out);
        encoder::pack(this->hash_length, out);
        encoder::pack(this->next_owner, false, out);
        encoder::pack(this->type_bitmap, this->type_bitmap.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void NSEC3PARAM::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->hash, out);
        encoder::pack(this->flags, out);
        encoder::pack(this->iterations, out);
        encoder::pack(this->salt_length, out);
        encoder::pack(this->salt, this->salt_length, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void TLSA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->usage, out);
        encoder::pack(this->selector, out);
        encoder::pack(this->matching_type, out);
        encoder::pack(this->certificate, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void SMIMEA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->usage, out);
        encoder::pack(this->selector, out);
        encoder::pack(this->matching_type, out);
        encoder::pack(this->certificate, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void HIP::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->hit_length, out);
        encoder::pack(this->pk_algorithm, out);
        encoder::pack(this->pk_length, out);
        encoder::pack(this->hit, false, out);
        encoder::pack(this->publickey, false, out);
        encoder::pack(this->rendezvous_servers, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void NINFO::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->zs_data, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NINFO::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->zs_data, false);
}


// -----------------------------------------------------------------------------
// RKEY
void RKEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->flags, out);
        encoder::pack(this->protocol, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->publickey, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void TALINK::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->previous_name, true, out);
        encoder::pack(this->next_name, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void TALINK::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->previous_name, true);
    decoder.unpack(this->next_name, true);
}


// -----------------------------------------------------------------------------
// CDS
void CDS::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->key_tag, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->digest_type, out);
        encoder::pack(this->digest, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void CDNSKEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->flags, out);
        encoder::pack(this->protocol, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->publickey, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void OPENPGPKEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->publickey, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void OPENPGPKEY::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->publickey, false);
}


// -----------------------------------------------------------------------------
// CSYNC
void CSYNC::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->serial, out);
        encoder::pack(this->flags, out);
        encoder::pack(this->type_bitmap, this->type_bitmap.size(), out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void SPF::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->txt, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void SPF::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->txt, false);
}


// -----------------------------------------------------------------------------
// NID
void NID::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->node_id, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void NID::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->node_id);
}


// -----------------------------------------------------------------------------
// L32
void L32::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->locator32, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void L32::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->locator32);
}


// -----------------------------------------------------------------------------
// L64
void L64::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->locator64, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void L64::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->locator64);
}


// -----------------------------------------------------------------------------
// LP
void LP::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->preference, out);
        encoder::pack(this->fqdn, true, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void LP::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->preference);
    decoder.unpack(this->fqdn, true);
}


// -----------------------------------------------------------------------------
// EUI48
void EUI48::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->address, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void EUI48::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// EUI64
void EUI64::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->address, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void EUI64::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->address);
}


// -----------------------------------------------------------------------------
// TKEY
void TKEY::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->algorithm, true, out);
        encoder::pack(this->inception, out);
        encoder::pack(this->expiration, out);
        encoder::pack(this->mode, out);
        encoder::pack(this->error, out);
        encoder::pack(this->key_size, out);
        encoder::pack(this->key, this->key_size, out);
        encoder::pack(this->other_len, out);
        encoder::pack(this->other_data, this->other_len, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void TSIG::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->algorithm, true, out);
        encoder::pack(this->time_signed, out);
        encoder::pack(this->fudge, out);
        encoder::pack(this->mac_size, out);
        encoder::pack(this->mac, this->mac_size, out);
        encoder::pack(this->original_id, out);
        encoder::pack(this->error, out);
        encoder::pack(this->other_len, out);
        encoder::pack(this->other_data, this->other_len, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void URI::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->priority, out);
        encoder::pack(this->weight, out);
        encoder::pack(this->target, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void CAA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->flags, out);
        encoder::pack(this->tag, false, out);
        encoder::pack(this->value, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void TA::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->key_tag, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->digest_type, out);
        encoder::pack(this->digest, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
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
void DLV::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::encode(out);

        // self
        auto val = out.size();

        encoder::pack(this->key_tag, out);
        encoder::pack(this->algorithm, out);
        encoder::pack(this->digest_type, out);
        encoder::pack(this->digest, false, out);

        // rdlength
        this->adjust(out, out.size() - val);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void DLV::decode(chen::dns::decoder &decoder)
{
    RR::decode(decoder);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, false);
}