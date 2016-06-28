/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_record.hpp>
#include <socket/dns/dns_error.hpp>
#include <socket/dns/dns_codec.hpp>
#include <socket/dns/dns_table.hpp>
#include <chen/base/num.hpp>
#include <limits>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR
RR::RR(chen::dns::RRType type) : rrtype(type)
{
}

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

// str
std::string RR::str(const std::string &sep) const
{
    std::string ret;

    ret += this->name + sep;
    ret += chen::num::str(this->ttl) + sep;
    ret += chen::dns::table::classToText(this->rrclass) + sep;
    ret += chen::dns::table::typeToText(this->rrtype) + sep;

    return ret;
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
// Raw
Raw::Raw() : Raw(chen::dns::RRType::None)
{
}

Raw::Raw(chen::dns::RRType type) : RR(type)
{
}

std::string Raw::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> Raw::clone() const
{
    return std::make_shared<Raw>(*this);
}

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
// Unknown
std::string Unknown::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> Unknown::clone() const
{
    return std::make_shared<Unknown>(*this);
}


// -----------------------------------------------------------------------------
// A
A::A() : RR(chen::dns::RRType::A)
{
}

std::string A::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> A::clone() const
{
    return std::make_shared<A>(*this);
}

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
NS::NS() : RR(chen::dns::RRType::NS)
{
}

std::string NS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NS::clone() const
{
    return std::make_shared<NS>(*this);
}

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
MD::MD() : RR(chen::dns::RRType::MD)
{
}

std::string MD::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MD::clone() const
{
    return std::make_shared<MD>(*this);
}

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
MF::MF() : RR(chen::dns::RRType::MF)
{
}

std::string MF::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MF::clone() const
{
    return std::make_shared<MF>(*this);
}

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
CNAME::CNAME() : RR(chen::dns::RRType::CNAME)
{
}

std::string CNAME::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> CNAME::clone() const
{
    return std::make_shared<CNAME>(*this);
}

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
SOA::SOA() : RR(chen::dns::RRType::SOA)
{
}

std::string SOA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SOA::clone() const
{
    return std::make_shared<SOA>(*this);
}

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
MB::MB() : RR(chen::dns::RRType::MB)
{
}

std::string MB::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MB::clone() const
{
    return std::make_shared<MB>(*this);
}

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
MG::MG() : RR(chen::dns::RRType::MG)
{
}

std::string MG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MG::clone() const
{
    return std::make_shared<MG>(*this);
}

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
MR::MR() : RR(chen::dns::RRType::MR)
{
}

std::string MR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MR::clone() const
{
    return std::make_shared<MR>(*this);
}

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
NUL::NUL() : Raw(chen::dns::RRType::NUL)
{
}

std::string NUL::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NUL::clone() const
{
    return std::make_shared<NUL>(*this);
}

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
WKS::WKS() : RR(chen::dns::RRType::WKS)
{
}

std::string WKS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> WKS::clone() const
{
    return std::make_shared<WKS>(*this);
}

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
PTR::PTR() : RR(chen::dns::RRType::PTR)
{
}

std::string PTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> PTR::clone() const
{
    return std::make_shared<PTR>(*this);
}

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
HINFO::HINFO() : RR(chen::dns::RRType::HINFO)
{
}

std::string HINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> HINFO::clone() const
{
    return std::make_shared<HINFO>(*this);
}

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
MINFO::MINFO() : RR(chen::dns::RRType::MINFO)
{
}

std::string MINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MINFO::clone() const
{
    return std::make_shared<MINFO>(*this);
}

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
MX::MX() : RR(chen::dns::RRType::MX)
{
}

std::string MX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> MX::clone() const
{
    return std::make_shared<MX>(*this);
}

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
TXT::TXT() : RR(chen::dns::RRType::TXT)
{
}

std::string TXT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> TXT::clone() const
{
    return std::make_shared<TXT>(*this);
}

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
RP::RP() : RR(chen::dns::RRType::RP)
{
}

std::string RP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> RP::clone() const
{
    return std::make_shared<RP>(*this);
}

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
AFSDB::AFSDB() : RR(chen::dns::RRType::AFSDB)
{
}

std::string AFSDB::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> AFSDB::clone() const
{
    return std::make_shared<AFSDB>(*this);
}

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
X25::X25() : RR(chen::dns::RRType::X25)
{
}

std::string X25::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> X25::clone() const
{
    return std::make_shared<X25>(*this);
}

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
ISDN::ISDN() : RR(chen::dns::RRType::ISDN)
{
}

std::string ISDN::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> ISDN::clone() const
{
    return std::make_shared<ISDN>(*this);
}

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
RT::RT() : RR(chen::dns::RRType::RT)
{
}

std::string RT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> RT::clone() const
{
    return std::make_shared<RT>(*this);
}

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
NSAP::NSAP() : RR(chen::dns::RRType::NSAP)
{
}

std::string NSAP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NSAP::clone() const
{
    return std::make_shared<NSAP>(*this);
}

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
NSAPPTR::NSAPPTR() : RR(chen::dns::RRType::NSAPPTR)
{
}

std::string NSAPPTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NSAPPTR::clone() const
{
    return std::make_shared<NSAPPTR>(*this);
}

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
SIG::SIG() : RR(chen::dns::RRType::SIG)
{
}

std::string SIG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SIG::clone() const
{
    return std::make_shared<SIG>(*this);
}

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
KEY::KEY() : RR(chen::dns::RRType::KEY)
{
}

std::string KEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> KEY::clone() const
{
    return std::make_shared<KEY>(*this);
}

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
PX::PX() : RR(chen::dns::RRType::PX)
{
}

std::string PX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> PX::clone() const
{
    return std::make_shared<PX>(*this);
}

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
GPOS::GPOS() : RR(chen::dns::RRType::GPOS)
{
}

std::string GPOS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> GPOS::clone() const
{
    return std::make_shared<GPOS>(*this);
}

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
AAAA::AAAA() : RR(chen::dns::RRType::AAAA)
{
}

std::string AAAA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> AAAA::clone() const
{
    return std::make_shared<AAAA>(*this);
}

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
LOC::LOC() : RR(chen::dns::RRType::LOC)
{
}

std::string LOC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> LOC::clone() const
{
    return std::make_shared<LOC>(*this);
}

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
NXT::NXT() : RR(chen::dns::RRType::NXT)
{
}

std::string NXT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NXT::clone() const
{
    return std::make_shared<NXT>(*this);
}

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
EID::EID() : RR(chen::dns::RRType::EID)
{
}

std::string EID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> EID::clone() const
{
    return std::make_shared<EID>(*this);
}

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
NIMLOC::NIMLOC() : RR(chen::dns::RRType::NIMLOC)
{
}

std::string NIMLOC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NIMLOC::clone() const
{
    return std::make_shared<NIMLOC>(*this);
}

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
SRV::SRV() : RR(chen::dns::RRType::SRV)
{
}

std::string SRV::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SRV::clone() const
{
    return std::make_shared<SRV>(*this);
}

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
ATMA::ATMA() : RR(chen::dns::RRType::ATMA)
{
}

std::string ATMA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> ATMA::clone() const
{
    return std::make_shared<ATMA>(*this);
}

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
NAPTR::NAPTR() : RR(chen::dns::RRType::NAPTR)
{
}

std::string NAPTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NAPTR::clone() const
{
    return std::make_shared<NAPTR>(*this);
}

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
KX::KX() : RR(chen::dns::RRType::KX)
{
}

std::string KX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> KX::clone() const
{
    return std::make_shared<KX>(*this);
}

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
CERT::CERT() : RR(chen::dns::RRType::CERT)
{
}

std::string CERT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> CERT::clone() const
{
    return std::make_shared<CERT>(*this);
}

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
// A6
std::string A6::str(const std::string &sep) const
{
    auto ret = AAAA::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> A6::clone() const
{
    return std::make_shared<A6>(*this);
}


// -----------------------------------------------------------------------------
// DNAME
DNAME::DNAME() : RR(chen::dns::RRType::DNAME)
{
}

std::string DNAME::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> DNAME::clone() const
{
    return std::make_shared<DNAME>(*this);
}

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
SINK::SINK() : RR(chen::dns::RRType::SINK)
{
}

std::string SINK::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SINK::clone() const
{
    return std::make_shared<SINK>(*this);
}

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
// OPT
OPT::OPT() : Raw(chen::dns::RRType::OPT)
{
}

std::string OPT::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> OPT::clone() const
{
    return std::make_shared<OPT>(*this);
}


// -----------------------------------------------------------------------------
// DS
DS::DS() : RR(chen::dns::RRType::DS)
{
}

std::string DS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> DS::clone() const
{
    return std::make_shared<DS>(*this);
}

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
SSHFP::SSHFP() : RR(chen::dns::RRType::SSHFP)
{
}

std::string SSHFP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SSHFP::clone() const
{
    return std::make_shared<SSHFP>(*this);
}

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
IPSECKEY::IPSECKEY() : RR(chen::dns::RRType::IPSECKEY)
{
}

std::string IPSECKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> IPSECKEY::clone() const
{
    return std::make_shared<IPSECKEY>(*this);
}

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
RRSIG::RRSIG() : RR(chen::dns::RRType::RRSIG)
{
}

std::string RRSIG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> RRSIG::clone() const
{
    return std::make_shared<RRSIG>(*this);
}

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
NSEC::NSEC() : RR(chen::dns::RRType::NSEC)
{
}

std::string NSEC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NSEC::clone() const
{
    return std::make_shared<NSEC>(*this);
}

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
DNSKEY::DNSKEY() : RR(chen::dns::RRType::DNSKEY)
{
}

std::string DNSKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> DNSKEY::clone() const
{
    return std::make_shared<DNSKEY>(*this);
}

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
DHCID::DHCID() : RR(chen::dns::RRType::DHCID)
{
}

std::string DHCID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> DHCID::clone() const
{
    return std::make_shared<DHCID>(*this);
}

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
NSEC3::NSEC3() : RR(chen::dns::RRType::NSEC3)
{
}

std::string NSEC3::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NSEC3::clone() const
{
    return std::make_shared<NSEC3>(*this);
}

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
NSEC3PARAM::NSEC3PARAM() : RR(chen::dns::RRType::NSEC3PARAM)
{
}

std::string NSEC3PARAM::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NSEC3PARAM::clone() const
{
    return std::make_shared<NSEC3PARAM>(*this);
}

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
TLSA::TLSA() : RR(chen::dns::RRType::TLSA)
{
}

std::string TLSA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> TLSA::clone() const
{
    return std::make_shared<TLSA>(*this);
}

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
SMIMEA::SMIMEA() : RR(chen::dns::RRType::SMIMEA)
{
}

std::string SMIMEA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SMIMEA::clone() const
{
    return std::make_shared<SMIMEA>(*this);
}

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
HIP::HIP() : RR(chen::dns::RRType::HIP)
{
}

std::string HIP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> HIP::clone() const
{
    return std::make_shared<HIP>(*this);
}

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
NINFO::NINFO() : RR(chen::dns::RRType::NINFO)
{
}

std::string NINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NINFO::clone() const
{
    return std::make_shared<NINFO>(*this);
}

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
RKEY::RKEY() : RR(chen::dns::RRType::RKEY)
{
}

std::string RKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> RKEY::clone() const
{
    return std::make_shared<RKEY>(*this);
}

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
TALINK::TALINK() : RR(chen::dns::RRType::TALINK)
{
}

std::string TALINK::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> TALINK::clone() const
{
    return std::make_shared<TALINK>(*this);
}

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
CDS::CDS() : RR(chen::dns::RRType::CDS)
{
}

std::string CDS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> CDS::clone() const
{
    return std::make_shared<CDS>(*this);
}

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
CDNSKEY::CDNSKEY() : RR(chen::dns::RRType::CDNSKEY)
{
}

std::string CDNSKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> CDNSKEY::clone() const
{
    return std::make_shared<CDNSKEY>(*this);
}

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
OPENPGPKEY::OPENPGPKEY() : RR(chen::dns::RRType::OPENPGPKEY)
{
}

std::string OPENPGPKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> OPENPGPKEY::clone() const
{
    return std::make_shared<OPENPGPKEY>(*this);
}

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
CSYNC::CSYNC() : RR(chen::dns::RRType::CSYNC)
{
}

std::string CSYNC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> CSYNC::clone() const
{
    return std::make_shared<CSYNC>(*this);
}

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
SPF::SPF() : RR(chen::dns::RRType::SPF)
{
}

std::string SPF::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> SPF::clone() const
{
    return std::make_shared<SPF>(*this);
}

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
// UINFO
UINFO::UINFO() : Raw(chen::dns::RRType::UINFO)
{
}

std::string UINFO::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> UINFO::clone() const
{
    return std::make_shared<UINFO>(*this);
}


// -----------------------------------------------------------------------------
// UID
UID::UID() : Raw(chen::dns::RRType::UID)
{
}

std::string UID::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> UID::clone() const
{
    return std::make_shared<UID>(*this);
}


// -----------------------------------------------------------------------------
// GID
GID::GID() : Raw(chen::dns::RRType::GID)
{
}

std::string GID::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> GID::clone() const
{
    return std::make_shared<GID>(*this);
}


// -----------------------------------------------------------------------------
// UNSPEC
UNSPEC::UNSPEC() : Raw(chen::dns::RRType::UNSPEC)
{
}

std::string UNSPEC::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> UNSPEC::clone() const
{
    return std::make_shared<UNSPEC>(*this);
}


// -----------------------------------------------------------------------------
// NID
NID::NID() : RR(chen::dns::RRType::NID)
{
}

std::string NID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> NID::clone() const
{
    return std::make_shared<NID>(*this);
}

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
L32::L32() : RR(chen::dns::RRType::L32)
{
}

std::string L32::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> L32::clone() const
{
    return std::make_shared<L32>(*this);
}

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
L64::L64() : RR(chen::dns::RRType::L64)
{
}

std::string L64::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> L64::clone() const
{
    return std::make_shared<L64>(*this);
}

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
LP::LP() : RR(chen::dns::RRType::LP)
{
}

std::string LP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> LP::clone() const
{
    return std::make_shared<LP>(*this);
}

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
EUI48::EUI48() : RR(chen::dns::RRType::EUI48)
{
}

std::string EUI48::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> EUI48::clone() const
{
    return std::make_shared<EUI48>(*this);
}

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
EUI64::EUI64() : RR(chen::dns::RRType::EUI64)
{
}

std::string EUI64::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> EUI64::clone() const
{
    return std::make_shared<EUI64>(*this);
}

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
TKEY::TKEY() : RR(chen::dns::RRType::TKEY)
{
}

std::string TKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> TKEY::clone() const
{
    return std::make_shared<TKEY>(*this);
}

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
TSIG::TSIG() : RR(chen::dns::RRType::TSIG)
{
}

std::string TSIG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> TSIG::clone() const
{
    return std::make_shared<TSIG>(*this);
}

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
URI::URI() : RR(chen::dns::RRType::URI)
{
}

std::string URI::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> URI::clone() const
{
    return std::make_shared<URI>(*this);
}

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
CAA::CAA() : RR(chen::dns::RRType::CAA)
{
}

std::string CAA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> CAA::clone() const
{
    return std::make_shared<CAA>(*this);
}

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
TA::TA() : RR(chen::dns::RRType::TA)
{
}

std::string TA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> TA::clone() const
{
    return std::make_shared<TA>(*this);
}

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
DLV::DLV() : RR(chen::dns::RRType::DLV)
{
}

std::string DLV::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    return ret;
}

std::shared_ptr<chen::dns::RR> DLV::clone() const
{
    return std::make_shared<DLV>(*this);
}

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