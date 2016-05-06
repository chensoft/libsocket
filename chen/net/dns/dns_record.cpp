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
#include <chen/base/str.hpp>
#include <limits>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR
RR::~RR()
{

}

// encode & decode
std::vector<std::uint8_t> RR::encode() const
{
    std::vector<std::uint8_t> out;
    this->pack(out);
    return out;
}

void RR::encode(std::vector<std::uint8_t> &out) const
{
    this->pack(out);
}

std::shared_ptr<chen::dns::RR> RR::decode(const std::vector<std::uint8_t> &data)
{
    auto cur = data.begin();
    auto end = data.end();
    return RR::decode(cur, end);
}

std::shared_ptr<chen::dns::RR> RR::decode(std::vector<std::uint8_t>::const_iterator &cur,
                                          std::vector<std::uint8_t>::const_iterator &end)
{
    // name
    std::string name;
    decoder::unpack(name, true, cur, end);

    // rrtype
    chen::dns::RRType rrtype = chen::dns::RRType::None;
    decoder::unpack(rrtype, cur, end);

    // rrclass
    chen::dns::RRClass rrclass = chen::dns::RRClass::IN;
    decoder::unpack(rrclass, cur, end);

    // ttl
    std::int32_t ttl = 0;
    decoder::unpack(ttl, cur, end);

    // build
    std::shared_ptr<chen::dns::RR> record = table::build(rrtype);
    if (!record)
        record.reset(new chen::dns::Unknown);

    // rdlength && rdata
    record->unpack(cur, end);

    // set
    record->name    = std::move(name);
    record->rrtype  = rrtype;
    record->rrclass = rrclass;
    record->ttl     = ttl;

    return record;
}

// pack & unpack
void RR::pack(std::vector<std::uint8_t> &out) const
{
    // common
    encoder::pack(this->name, true, out);
    encoder::pack(this->rrtype, out);
    encoder::pack(this->rrclass, out);
    encoder::pack(this->ttl, out);

    // rdlength, placeholder here
    encoder::pack(static_cast<std::uint16_t>(0), out);
}

void RR::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    decoder::unpack(this->rdlength, cur, end);
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

std::size_t RR::remain(std::vector<std::uint8_t>::const_iterator &beg,
                       std::vector<std::uint8_t>::const_iterator &cur) const
{
    auto used = cur - beg;
    if (this->rdlength < used)
        throw error_size("dns: codec rdata is overflow");

    return static_cast<std::size_t>(this->rdlength - used);
}


// -----------------------------------------------------------------------------
// RAW
void Raw::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void Raw::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->rdata, this->rdlength, cur, end);
}


// -----------------------------------------------------------------------------
// A
void A::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void A::unpack(std::vector<std::uint8_t>::const_iterator &cur,
               std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->address, cur, end);
}


// -----------------------------------------------------------------------------
// NS
void NS::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NS::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->nsdname, true, cur, end);
}


// -----------------------------------------------------------------------------
// MD
void MD::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MD::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->madname, true, cur, end);
}


// -----------------------------------------------------------------------------
// MF
void MF::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MF::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->madname, true, cur, end);
}


// -----------------------------------------------------------------------------
// CNAME
void CNAME::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void CNAME::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->cname, true, cur, end);
}


// -----------------------------------------------------------------------------
// SOA
void SOA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SOA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->mname, true, cur, end);
    decoder::unpack(this->rname, true, cur, end);
    decoder::unpack(this->serial, cur, end);
    decoder::unpack(this->refresh, cur, end);
    decoder::unpack(this->retry, cur, end);
    decoder::unpack(this->expire, cur, end);
    decoder::unpack(this->minimum, cur, end);
}


// -----------------------------------------------------------------------------
// MB
void MB::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MB::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->madname, true, cur, end);
}


// -----------------------------------------------------------------------------
// MG
void MG::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MG::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->mgmname, true, cur, end);
}


// -----------------------------------------------------------------------------
// MR
void MR::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MR::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->newname, true, cur, end);
}


// -----------------------------------------------------------------------------
// NUL
void NUL::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NUL::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->anything, this->rdlength, cur, end);
}


// -----------------------------------------------------------------------------
// WKS
void WKS::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void WKS::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    auto beg = cur;

    RR::unpack(cur, end);

    decoder::unpack(this->address, cur, end);
    decoder::unpack(this->protocol, cur, end);

    this->bitmap.clear();
    decoder::unpack(this->bitmap, this->remain(beg, cur), cur, end);
}


// -----------------------------------------------------------------------------
// PTR
void PTR::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void PTR::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->ptrdname, true, cur, end);
}


// -----------------------------------------------------------------------------
// HINFO
void HINFO::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void HINFO::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->cpu, false, cur, end);
    decoder::unpack(this->os, false, cur, end);
}


// -----------------------------------------------------------------------------
// MINFO
void MINFO::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MINFO::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->rmailbx, true, cur, end);
    decoder::unpack(this->emailbx, true, cur, end);
}


// -----------------------------------------------------------------------------
// MX
void MX::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void MX::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->exchange, true, cur, end);
}


// -----------------------------------------------------------------------------
// TXT
void TXT::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void TXT::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->txt_data, false, cur, end);
}


// -----------------------------------------------------------------------------
// RP
void RP::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void RP::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->mbox_dname, true, cur, end);
    decoder::unpack(this->txt_dname, true, cur, end);
}


// -----------------------------------------------------------------------------
// AFSDB
void AFSDB::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void AFSDB::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->subtype, cur, end);
    decoder::unpack(this->hostname, true, cur, end);
}


// -----------------------------------------------------------------------------
// X25
void X25::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void X25::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->psdn_address, false, cur, end);
}


// -----------------------------------------------------------------------------
// ISDN
void ISDN::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void ISDN::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->isdn_address, false, cur, end);
    decoder::unpack(this->sa, false, cur, end);
}


// -----------------------------------------------------------------------------
// RT
void RT::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void RT::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->intermediate_host, true, cur, end);
}


// -----------------------------------------------------------------------------
// NSAP
void NSAP::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NSAP::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->nsap, false, cur, end);
}


// -----------------------------------------------------------------------------
// NSAPPTR
void NSAPPTR::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NSAPPTR::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                     std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->owner, true, cur, end);
}


// -----------------------------------------------------------------------------
// SIG
void SIG::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SIG::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    decoder::unpack(this->type_covered, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->labels, cur, end);
    decoder::unpack(this->original, cur, end);
    decoder::unpack(this->expiration, cur, end);
    decoder::unpack(this->inception, cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->signer, true, cur, end);
    decoder::unpack(this->signature, false, cur, end);
}


// -----------------------------------------------------------------------------
// KEY
void KEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void KEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->protocol, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->publickey, false, cur, end);
}


// -----------------------------------------------------------------------------
// PX
void PX::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void PX::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->map822, true, cur, end);
    decoder::unpack(this->mapx400, true, cur, end);
}


// -----------------------------------------------------------------------------
// GPOS
void GPOS::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void GPOS::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->longitude, false, cur, end);
    decoder::unpack(this->latitude, false, cur, end);
    decoder::unpack(this->altitude, false, cur, end);
}


// -----------------------------------------------------------------------------
// AAAA
void AAAA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void AAAA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->address, cur, end);
}


// -----------------------------------------------------------------------------
// LOC
void LOC::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void LOC::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->version, cur, end);
    decoder::unpack(this->size, cur, end);
    decoder::unpack(this->horiz_pre, cur, end);
    decoder::unpack(this->vert_pre, cur, end);
    decoder::unpack(this->longitude, cur, end);
    decoder::unpack(this->latitude, cur, end);
    decoder::unpack(this->altitude, cur, end);
}


// -----------------------------------------------------------------------------
// NXT
void NXT::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NXT::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    auto beg = cur;

    RR::unpack(cur, end);
    decoder::unpack(this->next_domain, true, cur, end);

    this->type_bitmap.clear();
    decoder::unpack(this->type_bitmap, this->remain(beg, cur), cur, end);
}


// -----------------------------------------------------------------------------
// EID
void EID::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void EID::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->endpoint, false, cur, end);
}


// -----------------------------------------------------------------------------
// NIMLOC
void NIMLOC::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NIMLOC::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                    std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->locator, false, cur, end);
}


// -----------------------------------------------------------------------------
// SRV
void SRV::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SRV::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->priority, cur, end);
    decoder::unpack(this->weight, cur, end);
    decoder::unpack(this->port, cur, end);
    decoder::unpack(this->target, true, cur, end);
}


// -----------------------------------------------------------------------------
// ATMA
void ATMA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void ATMA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->format, cur, end);
    decoder::unpack(this->address, false, cur, end);
}


// -----------------------------------------------------------------------------
// NAPTR
void NAPTR::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NAPTR::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->order, cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->flags, false, cur, end);
    decoder::unpack(this->services, false, cur, end);
    decoder::unpack(this->regexp, false, cur, end);
    decoder::unpack(this->replacement, true, cur, end);
}


// -----------------------------------------------------------------------------
// KX
void KX::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void KX::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->exchanger, true, cur, end);
}


// -----------------------------------------------------------------------------
// CERT
void CERT::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void CERT::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->type, cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->certificate, false, cur, end);
}


// -----------------------------------------------------------------------------
// DNAME
void DNAME::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void DNAME::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->target, true, cur, end);
}


// -----------------------------------------------------------------------------
// SINK
void SINK::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SINK::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    auto beg = cur;

    RR::unpack(cur, end);
    decoder::unpack(this->coding, cur, end);
    decoder::unpack(this->subcoding, cur, end);

    this->sdata.clear();
    decoder::unpack(this->sdata, this->remain(beg, cur), cur, end);
}


// -----------------------------------------------------------------------------
// DS
void DS::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void DS::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->digest_type, cur, end);
    decoder::unpack(this->digest, false, cur, end);
}


// -----------------------------------------------------------------------------
// SSHFP
void SSHFP::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SSHFP::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->fptype, cur, end);
    decoder::unpack(this->fingerprint, false, cur, end);
}


// -----------------------------------------------------------------------------
// IPSECKEY
void IPSECKEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void IPSECKEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                      std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->precedence, cur, end);
    decoder::unpack(this->gateway_type, cur, end);
    decoder::unpack(this->algorithm, cur, end);

    this->gateway.clear();

    switch (static_cast<GatewayType>(this->gateway_type))
    {
        case GatewayType::None:
            decoder::unpack(this->gateway, 1, cur, end);
            break;

        case GatewayType::IPv4:
            decoder::unpack(this->gateway, 4, cur, end);
            break;

        case GatewayType::IPv6:
            decoder::unpack(this->gateway, 16, cur, end);
            break;

        case GatewayType::Domain:
        {
            std::string domain;
            decoder::unpack(domain, true, cur, end);

            this->gateway.insert(this->gateway.begin(), domain.begin(), domain.end());
        }
            break;
    }

    decoder::unpack(this->publickey, false, cur, end);
}


// -----------------------------------------------------------------------------
// RRSIG
void RRSIG::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void RRSIG::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->type_covered, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->labels, cur, end);
    decoder::unpack(this->original, cur, end);
    decoder::unpack(this->expiration, cur, end);
    decoder::unpack(this->inception, cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->signer, true, cur, end);
    decoder::unpack(this->signature, false, cur, end);
}


// -----------------------------------------------------------------------------
// NSEC
void NSEC::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NSEC::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    auto beg = cur;

    RR::unpack(cur, end);
    decoder::unpack(this->next_domain, true, cur, end);

    this->type_bitmap.clear();
    decoder::unpack(this->type_bitmap, this->remain(beg, cur), cur, end);
}


// -----------------------------------------------------------------------------
// DNSKEY
void DNSKEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void DNSKEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                    std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->protocol, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->publickey, false, cur, end);
}


// -----------------------------------------------------------------------------
// DHCID
void DHCID::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void DHCID::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->digest, false, cur, end);
}


// -----------------------------------------------------------------------------
// NSEC3
void NSEC3::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NSEC3::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    auto beg = cur;

    RR::unpack(cur, end);
    decoder::unpack(this->hash, cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->iterations, cur, end);
    decoder::unpack(this->salt_length, cur, end);
    decoder::unpack(this->salt, this->salt_length, cur, end);
    decoder::unpack(this->hash_length, cur, end);
    decoder::unpack(this->next_owner, false, cur, end);

    this->type_bitmap.clear();
    decoder::unpack(this->type_bitmap, this->remain(beg, cur), cur, end);
}


// -----------------------------------------------------------------------------
// NSEC3PARAM
void NSEC3PARAM::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NSEC3PARAM::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                        std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->hash, cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->iterations, cur, end);
    decoder::unpack(this->salt_length, cur, end);
    decoder::unpack(this->salt, this->salt_length, cur, end);
}


// -----------------------------------------------------------------------------
// TLSA
void TLSA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void TLSA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->usage, cur, end);
    decoder::unpack(this->selector, cur, end);
    decoder::unpack(this->matching_type, cur, end);
    decoder::unpack(this->certificate, false, cur, end);
}


// -----------------------------------------------------------------------------
// SMIMEA
void SMIMEA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SMIMEA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                    std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->usage, cur, end);
    decoder::unpack(this->selector, cur, end);
    decoder::unpack(this->matching_type, cur, end);
    decoder::unpack(this->certificate, false, cur, end);
}


// -----------------------------------------------------------------------------
// HIP
void HIP::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void HIP::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->hit_length, cur, end);
    decoder::unpack(this->pk_algorithm, cur, end);
    decoder::unpack(this->pk_length, cur, end);
    decoder::unpack(this->hit, false, cur, end);
    decoder::unpack(this->publickey, false, cur, end);
    decoder::unpack(this->rendezvous_servers, false, cur, end);
}


// -----------------------------------------------------------------------------
// NINFO
void NINFO::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NINFO::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->zs_data, false, cur, end);
}


// -----------------------------------------------------------------------------
// RKEY
void RKEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void RKEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->protocol, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->publickey, false, cur, end);
}


// -----------------------------------------------------------------------------
// TALINK
void TALINK::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void TALINK::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                    std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->previous_name, true, cur, end);
    decoder::unpack(this->next_name, true, cur, end);
}


// -----------------------------------------------------------------------------
// CDS
void CDS::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void CDS::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->digest_type, cur, end);
    decoder::unpack(this->digest, false, cur, end);
}


// -----------------------------------------------------------------------------
// CDNSKEY
void CDNSKEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void CDNSKEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                     std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->protocol, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->publickey, false, cur, end);
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
void OPENPGPKEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void OPENPGPKEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                        std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->publickey, false, cur, end);
}


// -----------------------------------------------------------------------------
// CSYNC
void CSYNC::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void CSYNC::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    auto beg = cur;

    RR::unpack(cur, end);
    decoder::unpack(this->serial, cur, end);
    decoder::unpack(this->flags, cur, end);

    this->type_bitmap.clear();
    decoder::unpack(this->type_bitmap, this->remain(beg, cur), cur, end);
}


// -----------------------------------------------------------------------------
// SPF
void SPF::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void SPF::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->txt, false, cur, end);
}


// -----------------------------------------------------------------------------
// NID
void NID::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void NID::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->node_id, cur, end);
}


// -----------------------------------------------------------------------------
// L32
void L32::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void L32::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->locator32, cur, end);
}


// -----------------------------------------------------------------------------
// L64
void L64::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void L64::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->locator64, cur, end);
}


// -----------------------------------------------------------------------------
// LP
void LP::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void LP::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->preference, cur, end);
    decoder::unpack(this->fqdn, true, cur, end);
}


// -----------------------------------------------------------------------------
// EUI48
void EUI48::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void EUI48::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->address, cur, end);
}


// -----------------------------------------------------------------------------
// EUI64
void EUI64::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void EUI64::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                   std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->address, cur, end);
}


// -----------------------------------------------------------------------------
// TKEY
void TKEY::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void TKEY::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->algorithm, true, cur, end);
    decoder::unpack(this->inception, cur, end);
    decoder::unpack(this->expiration, cur, end);
    decoder::unpack(this->mode, cur, end);
    decoder::unpack(this->error, cur, end);
    decoder::unpack(this->key_size, cur, end);
    decoder::unpack(this->key, this->key_size, cur, end);
    decoder::unpack(this->other_len, cur, end);
    decoder::unpack(this->other_data, this->other_len, cur, end);
}


// -----------------------------------------------------------------------------
// TSIG
void TSIG::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void TSIG::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                  std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->algorithm, true, cur, end);
    decoder::unpack(this->time_signed, cur, end);
    decoder::unpack(this->fudge, cur, end);
    decoder::unpack(this->mac_size, cur, end);
    decoder::unpack(this->mac, this->mac_size, cur, end);
    decoder::unpack(this->original_id, cur, end);
    decoder::unpack(this->error, cur, end);
    decoder::unpack(this->other_len, cur, end);
    decoder::unpack(this->other_data, this->other_len, cur, end);
}


// -----------------------------------------------------------------------------
// URI
void URI::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void URI::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->priority, cur, end);
    decoder::unpack(this->weight, cur, end);
    decoder::unpack(this->target, false, cur, end);
}


// -----------------------------------------------------------------------------
// CAA
void CAA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void CAA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->flags, cur, end);
    decoder::unpack(this->tag, false, cur, end);
    decoder::unpack(this->value, false, cur, end);
}


// -----------------------------------------------------------------------------
// TA
void TA::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void TA::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->digest_type, cur, end);
    decoder::unpack(this->digest, false, cur, end);
}


// -----------------------------------------------------------------------------
// DLV
void DLV::pack(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // base
        RR::pack(out);

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

void DLV::unpack(std::vector<std::uint8_t>::const_iterator &cur,
                 std::vector<std::uint8_t>::const_iterator &end)
{
    RR::unpack(cur, end);
    decoder::unpack(this->key_tag, cur, end);
    decoder::unpack(this->algorithm, cur, end);
    decoder::unpack(this->digest_type, cur, end);
    decoder::unpack(this->digest, false, cur, end);
}