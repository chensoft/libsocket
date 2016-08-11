/**
 * Created by Jian Chen
 * @since  2015.12.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_record.hpp>
#include <socket/dns/dns_table.hpp>
#include <socket/dns/dns_codec.hpp>
#include <socket/ip/ip_address.hpp>
#include <chen/base/num.hpp>
#include <chen/base/map.hpp>
#include <limits>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// RR
RR::RR(chen::dns::RRType type) : rrtype(type)
{
}

// encode & decode
void RR::encode(chen::dns::encoder &encoder) const
{
    // according to rfc1035, section 4.1.4
    // pointers can only be used for occurance of a domain name where the format is not class specific
    // so I compress RR's name only
    // common
    encoder.pack(this->name, codec::StringType::Domain, true);
    encoder.pack(this->rrtype);
    encoder.pack(this->rrclass);
    encoder.pack(this->ttl);

    // rdlength, placeholder here
    encoder.pack(static_cast<std::uint16_t>(0));

    // current size
    auto before = encoder.size();

    // encode subclass
    this->pack(encoder);

    // rdlength
    auto len = encoder.size();
    auto off = len - before;

    if (off > std::numeric_limits<std::uint16_t>::max())
        throw error_codec("dns: codec pack rdata size is overflow");

    auto tmp = static_cast<std::uint16_t>(off);

    encoder.change(len - tmp - 2, static_cast<std::uint8_t>(tmp >> 8 & 0xFF));
    encoder.change(len - tmp - 1, static_cast<std::uint8_t>(tmp & 0xFF));
}

void RR::decode(chen::dns::decoder &decoder)
{
    // unpack base fields
    decoder.unpack(this->name, codec::StringType::Domain);
    decoder.unpack(this->rrtype);
    decoder.unpack(this->rrclass);
    decoder.unpack(this->ttl);
    decoder.unpack(this->rdlength);

    // unpack subclass
    this->unpack(decoder);
}

void RR::decode(const chen::json::object &object)
{
    // unpack base fields, ignore rdlength
    map::write(object, "name", this->name);
    map::write(object, "rrtype", reinterpret_cast<std::uint16_t&>(this->rrtype));
    map::write(object, "rrclass", reinterpret_cast<std::uint16_t&>(this->rrclass));
    map::write(object, "ttl", this->ttl);

    // unpack subclass
    this->unpack(object);
}

std::shared_ptr<chen::dns::RR> RR::create(chen::dns::decoder &decoder)
{
    chen::dns::decoder detect(decoder);

    // detect type
    std::string unused;
    detect.unpack(unused, codec::StringType::Domain);

    chen::dns::RRType rrtype = chen::dns::RRType::None;
    detect.unpack(rrtype);

    // build record
    std::shared_ptr<chen::dns::RR> record = table::build(rrtype);
    if (!record)
        return nullptr;

    // decode it
    record->decode(decoder);

    return record;
}

// str
std::string RR::str(const std::string &sep) const
{
    std::string ret;

    ret += this->name + sep;
    ret += num::str(this->ttl) + sep;
    ret += chen::dns::table::classToText(this->rrclass) + sep;
    ret += chen::dns::table::typeToText(this->rrtype);

    return ret;
}

// helper
std::size_t RR::remain(std::size_t used) const
{
    if (this->rdlength < used)
        throw error_codec("dns: codec rdata is overflow");

    return static_cast<std::size_t>(this->rdlength - used);
}

std::string RR::escape(const std::vector<std::uint8_t> &data)
{
    std::string ret;

    for (auto ch : data)
        ret += str::format("%02X", ch);

    return ret;
}

std::string RR::escape(const std::string &text)
{
    std::string ret("\"");

    // escape all non-printable characters
    for (auto ch : text)
    {
        if (std::isprint(ch))
            ret += ch;
        else
            ret += str::format("\\%03u", static_cast<unsigned char>(ch));
    }

    ret += '"';

    return ret;
}

std::string RR::escape(std::size_t bits)
{
    return "<<" + num::str(bits) + "bits>>";
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
    ret += sep + RR::escape(this->rdata.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> Raw::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void Raw::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->rdata, this->rdata.size());
}

void Raw::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->rdata, this->rdlength);
}

void Raw::unpack(const chen::json::object &object)
{
    this->rdata.clear();

    std::string rdata = map::find(object, "rdata");
    std::copy(rdata.begin(), rdata.end(), this->rdata.begin());
}


// -----------------------------------------------------------------------------
// A
A::A() : RR(chen::dns::RRType::A)
{
}

std::string A::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + chen::ip::version4::toString(this->address);
    return ret;
}

std::shared_ptr<chen::dns::RR> A::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void A::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void A::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void A::unpack(const chen::json::object &object)
{
    // if address is string then use address_v4::toInteger
    auto address = map::find(object, "address");

    if (address.isString())
        this->address = chen::ip::version4::toInteger(address);
    else
        this->address = address.toUnsigned();
}


// -----------------------------------------------------------------------------
// NS
NS::NS() : RR(chen::dns::RRType::NS)
{
}

std::string NS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->nsdname;
    return ret;
}

std::shared_ptr<chen::dns::RR> NS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NS::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->nsdname, codec::StringType::Domain, false);
}

void NS::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->nsdname, codec::StringType::Domain);
}

void NS::unpack(const chen::json::object &object)
{
    map::write(object, "nsdname", this->nsdname);
}


// -----------------------------------------------------------------------------
// MD
MD::MD() : RR(chen::dns::RRType::MD)
{
}

std::string MD::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->madname;
    return ret;
}

std::shared_ptr<chen::dns::RR> MD::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MD::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void MD::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void MD::unpack(const chen::json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// MF
MF::MF() : RR(chen::dns::RRType::MF)
{
}

std::string MF::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += this->madname;
    return ret;
}

std::shared_ptr<chen::dns::RR> MF::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MF::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void MF::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void MF::unpack(const chen::json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// CNAME
CNAME::CNAME() : RR(chen::dns::RRType::CNAME)
{
}

std::string CNAME::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->cname;
    return ret;
}

std::shared_ptr<chen::dns::RR> CNAME::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CNAME::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->cname, codec::StringType::Domain, false);
}

void CNAME::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->cname, codec::StringType::Domain);
}

void CNAME::unpack(const chen::json::object &object)
{
    map::write(object, "cname", this->cname);
}


// -----------------------------------------------------------------------------
// SOA
SOA::SOA() : RR(chen::dns::RRType::SOA)
{
}

std::string SOA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->mname;
    ret += " " + this->rname;  // use a space to seprate different fields
    ret += " " + num::str(this->serial);
    ret += " " + num::str(this->refresh);
    ret += " " + num::str(this->retry);
    ret += " " + num::str(this->expire);
    ret += " " + num::str(this->minimum);

    return ret;
}

std::shared_ptr<chen::dns::RR> SOA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SOA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->mname, codec::StringType::Domain, false);
    encoder.pack(this->rname, codec::StringType::Domain, false);
    encoder.pack(this->serial);
    encoder.pack(this->refresh);
    encoder.pack(this->retry);
    encoder.pack(this->expire);
    encoder.pack(this->minimum);
}

void SOA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->mname, codec::StringType::Domain);
    decoder.unpack(this->rname, codec::StringType::Domain);
    decoder.unpack(this->serial);
    decoder.unpack(this->refresh);
    decoder.unpack(this->retry);
    decoder.unpack(this->expire);
    decoder.unpack(this->minimum);
}

void SOA::unpack(const chen::json::object &object)
{
    map::write(object, "mname", this->mname);
    map::write(object, "rname", this->rname);
    map::write(object, "serial", this->serial);
    map::write(object, "refresh", this->refresh);
    map::write(object, "retry", this->retry);
    map::write(object, "expire", this->expire);
    map::write(object, "minimum", this->minimum);
}


// -----------------------------------------------------------------------------
// MB
MB::MB() : RR(chen::dns::RRType::MB)
{
}

std::string MB::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->madname;
    return ret;
}

std::shared_ptr<chen::dns::RR> MB::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MB::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void MB::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void MB::unpack(const chen::json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// MG
MG::MG() : RR(chen::dns::RRType::MG)
{
}

std::string MG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->mgmname;
    return ret;
}

std::shared_ptr<chen::dns::RR> MG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MG::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->mgmname, codec::StringType::Domain, false);
}

void MG::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->mgmname, codec::StringType::Domain);
}

void MG::unpack(const chen::json::object &object)
{
    map::write(object, "mgmname", this->mgmname);
}


// -----------------------------------------------------------------------------
// MR
MR::MR() : RR(chen::dns::RRType::MR)
{
}

std::string MR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->newname;
    return ret;
}

std::shared_ptr<chen::dns::RR> MR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MR::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->newname, codec::StringType::Domain, false);
}

void MR::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->newname, codec::StringType::Domain);
}

void MR::unpack(const chen::json::object &object)
{
    map::write(object, "newname", this->newname);
}


// -----------------------------------------------------------------------------
// Null
Null::Null() : Raw(chen::dns::RRType::Null)
{
}

std::string Null::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    ret += sep + RR::escape(this->anything.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> Null::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void Null::pack(chen::dns::encoder &encoder) const
{
    // base
    Raw::pack(encoder);

    // self
    auto val = encoder.size();

    encoder.pack(this->anything, this->anything.size());
}

void Null::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->anything, this->rdlength);
}

void Null::unpack(const chen::json::object &object)
{
    Raw::unpack(object);

    this->anything.clear();

    std::string anything = map::find(object, "anything", std::string());
    std::copy(anything.begin(), anything.end(), this->anything.begin());
}


// -----------------------------------------------------------------------------
// WKS
WKS::WKS() : RR(chen::dns::RRType::WKS)
{
}

std::string WKS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->address);
    ret += " " + num::str(this->protocol);
    ret += " " + RR::escape(this->bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> WKS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void WKS::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
    encoder.pack(this->protocol);
    encoder.pack(this->bitmap, this->bitmap.size());
}

void WKS::unpack(chen::dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->address);
    decoder.unpack(this->protocol);

    this->bitmap.clear();
    decoder.unpack(this->bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void WKS::unpack(const chen::json::object &object)
{
    map::write(object, "address", this->address);
    map::write(object, "protocol", this->protocol);

    this->bitmap.clear();

    std::string bitmap = map::find(object, "bitmap", std::string());
    std::copy(bitmap.begin(), bitmap.end(), this->bitmap.begin());
}


// -----------------------------------------------------------------------------
// PTR
PTR::PTR() : RR(chen::dns::RRType::PTR)
{
}

std::string PTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->ptrdname;
    return ret;
}

std::shared_ptr<chen::dns::RR> PTR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void PTR::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->ptrdname, codec::StringType::Domain, false);
}

void PTR::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->ptrdname, codec::StringType::Domain);
}

void PTR::unpack(const chen::json::object &object)
{
    map::write(object, "ptrdname", this->ptrdname);
}


// -----------------------------------------------------------------------------
// HINFO
HINFO::HINFO() : RR(chen::dns::RRType::HINFO)
{
}

std::string HINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->cpu;
    ret += " " + this->os;
    return ret;
}

std::shared_ptr<chen::dns::RR> HINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void HINFO::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->cpu, codec::StringType::Plain, false);
    encoder.pack(this->os, codec::StringType::Plain, false);
}

void HINFO::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->cpu, codec::StringType::Plain);
    decoder.unpack(this->os, codec::StringType::Plain);
}

void HINFO::unpack(const chen::json::object &object)
{
    map::write(object, "cpu", this->cpu);
    map::write(object, "os", this->os);
}


// -----------------------------------------------------------------------------
// MINFO
MINFO::MINFO() : RR(chen::dns::RRType::MINFO)
{
}

std::string MINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->rmailbx;
    ret += " " + this->emailbx;
    return ret;
}

std::shared_ptr<chen::dns::RR> MINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MINFO::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->rmailbx, codec::StringType::Domain, false);
    encoder.pack(this->emailbx, codec::StringType::Domain, false);
}

void MINFO::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->rmailbx, codec::StringType::Domain);
    decoder.unpack(this->emailbx, codec::StringType::Domain);
}

void MINFO::unpack(const chen::json::object &object)
{
    map::write(object, "rmailbx", this->rmailbx);
    map::write(object, "emailbx", this->emailbx);
}


// -----------------------------------------------------------------------------
// MX
MX::MX() : RR(chen::dns::RRType::MX)
{
}

std::string MX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->preference);
    ret += " " + this->exchange;
    return ret;
}

std::shared_ptr<chen::dns::RR> MX::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void MX::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchange, codec::StringType::Domain, false);
}

void MX::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->exchange, codec::StringType::Domain);
}

void MX::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "exchange", this->exchange);
}


// -----------------------------------------------------------------------------
// TXT
TXT::TXT() : RR(chen::dns::RRType::TXT)
{
}

std::string TXT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->txt_data);
    return ret;
}

std::shared_ptr<chen::dns::RR> TXT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void TXT::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->txt_data, codec::StringType::Plain, false);
}

void TXT::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->txt_data, codec::StringType::Plain);
}

void TXT::unpack(const chen::json::object &object)
{
    map::write(object, "txt_data", this->txt_data);
}


// -----------------------------------------------------------------------------
// RP
RP::RP() : RR(chen::dns::RRType::RP)
{
}

std::string RP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->mbox_dname;
    ret += " " + this->txt_dname;
    return ret;
}

std::shared_ptr<chen::dns::RR> RP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void RP::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->mbox_dname, codec::StringType::Domain, false);
    encoder.pack(this->txt_dname, codec::StringType::Domain, false);
}

void RP::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->mbox_dname, codec::StringType::Domain);
    decoder.unpack(this->txt_dname, codec::StringType::Domain);
}

void RP::unpack(const chen::json::object &object)
{
    map::write(object, "mbox_dname", this->mbox_dname);
    map::write(object, "txt_dname", this->txt_dname);
}


// -----------------------------------------------------------------------------
// AFSDB
AFSDB::AFSDB() : RR(chen::dns::RRType::AFSDB)
{
}

std::string AFSDB::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->subtype);
    ret += " " + this->hostname;
    return ret;
}

std::shared_ptr<chen::dns::RR> AFSDB::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void AFSDB::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->subtype);
    encoder.pack(this->hostname, codec::StringType::Domain, false);
}

void AFSDB::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->subtype);
    decoder.unpack(this->hostname, codec::StringType::Domain);
}

void AFSDB::unpack(const chen::json::object &object)
{
    map::write(object, "subtype", this->subtype);
    map::write(object, "hostname", this->hostname);
}


// -----------------------------------------------------------------------------
// X25
X25::X25() : RR(chen::dns::RRType::X25)
{
}

std::string X25::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->psdn_address;
    return ret;
}

std::shared_ptr<chen::dns::RR> X25::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void X25::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->psdn_address, codec::StringType::Plain, false);
}

void X25::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->psdn_address, codec::StringType::Plain);
}

void X25::unpack(const chen::json::object &object)
{
    map::write(object, "psdn_address", this->psdn_address);
}


// -----------------------------------------------------------------------------
// ISDN
ISDN::ISDN() : RR(chen::dns::RRType::ISDN)
{
}

std::string ISDN::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->isdn_address;
    ret += " " + this->sa;
    return ret;
}

std::shared_ptr<chen::dns::RR> ISDN::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void ISDN::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->isdn_address, codec::StringType::Plain, false);
    encoder.pack(this->sa, codec::StringType::Plain, false);
}

void ISDN::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->isdn_address, codec::StringType::Plain);
    decoder.unpack(this->sa, codec::StringType::Plain);
}

void ISDN::unpack(const chen::json::object &object)
{
    map::write(object, "isdn_address", this->isdn_address);
    map::write(object, "sa", this->sa);
}


// -----------------------------------------------------------------------------
// RT
RT::RT() : RR(chen::dns::RRType::RT)
{
}

std::string RT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->preference);
    ret += " " + this->intermediate_host;
    return ret;
}

std::shared_ptr<chen::dns::RR> RT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void RT::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->intermediate_host, codec::StringType::Domain, false);
}

void RT::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->intermediate_host, codec::StringType::Domain);
}

void RT::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "intermediate_host", this->intermediate_host);
}


// -----------------------------------------------------------------------------
// NSAP
NSAP::NSAP() : RR(chen::dns::RRType::NSAP)
{
}

std::string NSAP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->nsap;
    return ret;
}

std::shared_ptr<chen::dns::RR> NSAP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NSAP::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->nsap, codec::StringType::Plain, false);
}

void NSAP::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->nsap, codec::StringType::Plain);
}

void NSAP::unpack(const chen::json::object &object)
{
    map::write(object, "nsap", this->nsap);
}


// -----------------------------------------------------------------------------
// NSAPPTR
NSAPPTR::NSAPPTR() : RR(chen::dns::RRType::NSAPPTR)
{
}

std::string NSAPPTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->owner;
    return ret;
}

std::shared_ptr<chen::dns::RR> NSAPPTR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NSAPPTR::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->owner, codec::StringType::Domain, false);
}

void NSAPPTR::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->owner, codec::StringType::Domain);
}

void NSAPPTR::unpack(const chen::json::object &object)
{
    map::write(object, "owner", this->owner);
}


// -----------------------------------------------------------------------------
// SIG
SIG::SIG() : RR(chen::dns::RRType::SIG)
{
}

std::string SIG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->type_covered);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->labels);
    ret += " " + num::str(this->original);
    ret += " " + num::str(this->expiration);
    ret += " " + num::str(this->inception);
    ret += " " + num::str(this->key_tag);
    ret += " " + this->signer;
    ret += " " + this->signature;

    return ret;
}

std::shared_ptr<chen::dns::RR> SIG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SIG::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->type_covered);
    encoder.pack(this->algorithm);
    encoder.pack(this->labels);
    encoder.pack(this->original);
    encoder.pack(this->expiration);
    encoder.pack(this->inception);
    encoder.pack(this->key_tag);
    encoder.pack(this->signer, codec::StringType::Domain, false);
    encoder.pack(this->signature, codec::StringType::Plain, false);
}

void SIG::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->type_covered);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->labels);
    decoder.unpack(this->original);
    decoder.unpack(this->expiration);
    decoder.unpack(this->inception);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->signer, codec::StringType::Domain);
    decoder.unpack(this->signature, codec::StringType::Plain);
}

void SIG::unpack(const chen::json::object &object)
{
    map::write(object, "type_covered", this->type_covered);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "labels", this->labels);
    map::write(object, "original", this->original);
    map::write(object, "expiration", this->expiration);
    map::write(object, "inception", this->inception);
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "signer", this->signer);
    map::write(object, "signature", this->signature);
}


// -----------------------------------------------------------------------------
// KEY
KEY::KEY() : RR(chen::dns::RRType::KEY)
{
}

std::string KEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> KEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void KEY::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void KEY::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void KEY::unpack(const chen::json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// PX
PX::PX() : RR(chen::dns::RRType::PX)
{
}

std::string PX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->map822;
    ret += " " + this->mapx400;

    return ret;
}

std::shared_ptr<chen::dns::RR> PX::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void PX::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->map822, codec::StringType::Domain, false);
    encoder.pack(this->mapx400, codec::StringType::Domain, false);
}

void PX::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->map822, codec::StringType::Domain);
    decoder.unpack(this->mapx400, codec::StringType::Domain);
}

void PX::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "map822", this->map822);
    map::write(object, "mapx400", this->mapx400);
}


// -----------------------------------------------------------------------------
// GPOS
GPOS::GPOS() : RR(chen::dns::RRType::GPOS)
{
}

std::string GPOS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->longitude;
    ret += " " + this->latitude;
    ret += " " + this->altitude;

    return ret;
}

std::shared_ptr<chen::dns::RR> GPOS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void GPOS::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->longitude, codec::StringType::Plain, false);
    encoder.pack(this->latitude, codec::StringType::Plain, false);
    encoder.pack(this->altitude, codec::StringType::Plain, false);
}

void GPOS::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->longitude, codec::StringType::Plain);
    decoder.unpack(this->latitude, codec::StringType::Plain);
    decoder.unpack(this->altitude, codec::StringType::Plain);
}

void GPOS::unpack(const chen::json::object &object)
{
    map::write(object, "longitude", this->longitude);
    map::write(object, "latitude", this->latitude);
    map::write(object, "altitude", this->altitude);
}


// -----------------------------------------------------------------------------
// AAAA
AAAA::AAAA() : RR(chen::dns::RRType::AAAA)
{
}

std::string AAAA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + chen::ip::version6::toString(this->address.data());
    return ret;
}

std::shared_ptr<chen::dns::RR> AAAA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void AAAA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void AAAA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void AAAA::unpack(const chen::json::object &object)
{
    auto address = map::find(object, "address");
    this->address = chen::ip::version6::toBytes(address);
}


// -----------------------------------------------------------------------------
// LOC
LOC::LOC() : RR(chen::dns::RRType::LOC)
{
}

std::string LOC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->version);
    ret += " " + num::str(this->size);
    ret += " " + num::str(this->horiz_pre);
    ret += " " + num::str(this->vert_pre);
    ret += " " + num::str(this->longitude);
    ret += " " + num::str(this->latitude);
    ret += " " + num::str(this->altitude);

    return ret;
}

std::shared_ptr<chen::dns::RR> LOC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void LOC::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->size);
    encoder.pack(this->horiz_pre);
    encoder.pack(this->vert_pre);
    encoder.pack(this->longitude);
    encoder.pack(this->latitude);
    encoder.pack(this->altitude);
}

void LOC::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->version);
    decoder.unpack(this->size);
    decoder.unpack(this->horiz_pre);
    decoder.unpack(this->vert_pre);
    decoder.unpack(this->longitude);
    decoder.unpack(this->latitude);
    decoder.unpack(this->altitude);
}

void LOC::unpack(const chen::json::object &object)
{
    map::write(object, "version", this->version);
    map::write(object, "size", this->size);
    map::write(object, "horiz_pre", this->horiz_pre);
    map::write(object, "vert_pre", this->vert_pre);
    map::write(object, "longitude", this->longitude);
    map::write(object, "latitude", this->latitude);
    map::write(object, "altitude", this->altitude);
}


// -----------------------------------------------------------------------------
// NXT
NXT::NXT() : RR(chen::dns::RRType::NXT)
{
}

std::string NXT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->next_domain;
    ret += " " + RR::escape(this->type_bitmap.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> NXT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NXT::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, codec::StringType::Domain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void NXT::unpack(chen::dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->next_domain, codec::StringType::Domain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void NXT::unpack(const chen::json::object &object)
{
    map::write(object, "next_domain", this->next_domain);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// EID
EID::EID() : RR(chen::dns::RRType::EID)
{
}

std::string EID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->endpoint;
    return ret;
}

std::shared_ptr<chen::dns::RR> EID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void EID::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->endpoint, codec::StringType::Plain, false);
}

void EID::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->endpoint, codec::StringType::Plain);
}

void EID::unpack(const chen::json::object &object)
{
    map::write(object, "endpoint", this->endpoint);
}


// -----------------------------------------------------------------------------
// NIMLOC
NIMLOC::NIMLOC() : RR(chen::dns::RRType::NIMLOC)
{
}

std::string NIMLOC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->locator;
    return ret;
}

std::shared_ptr<chen::dns::RR> NIMLOC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NIMLOC::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->locator, codec::StringType::Plain, false);
}

void NIMLOC::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->locator, codec::StringType::Plain);
}

void NIMLOC::unpack(const chen::json::object &object)
{
    map::write(object, "locator", this->locator);
}


// -----------------------------------------------------------------------------
// SRV
SRV::SRV() : RR(chen::dns::RRType::SRV)
{
}

std::string SRV::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->priority);
    ret += " " + num::str(this->weight);
    ret += " " + num::str(this->port);
    ret += " " + this->target;

    return ret;
}

std::shared_ptr<chen::dns::RR> SRV::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SRV::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->port);
    encoder.pack(this->target, codec::StringType::Domain, false);
}

void SRV::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->port);
    decoder.unpack(this->target, codec::StringType::Domain);
}

void SRV::unpack(const chen::json::object &object)
{
    map::write(object, "priority", this->priority);
    map::write(object, "weight", this->weight);
    map::write(object, "port", this->port);
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// ATMA
ATMA::ATMA() : RR(chen::dns::RRType::ATMA)
{
}

std::string ATMA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->format);
    ret += " " + this->address;

    return ret;
}

std::shared_ptr<chen::dns::RR> ATMA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void ATMA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->format);
    encoder.pack(this->address, codec::StringType::Plain, false);
}

void ATMA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->format);
    decoder.unpack(this->address, codec::StringType::Plain);
}

void ATMA::unpack(const chen::json::object &object)
{
    map::write(object, "format", this->format);
    map::write(object, "address", this->address);
}


// -----------------------------------------------------------------------------
// NAPTR
NAPTR::NAPTR() : RR(chen::dns::RRType::NAPTR)
{
}

std::string NAPTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->order);
    ret += " " + num::str(this->preference);
    ret += " " + this->flags;
    ret += " " + this->services;
    ret += " " + this->regexp;
    ret += " " + this->replacement;

    return ret;
}

std::shared_ptr<chen::dns::RR> NAPTR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NAPTR::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->order);
    encoder.pack(this->preference);
    encoder.pack(this->flags, codec::StringType::Plain, false);
    encoder.pack(this->services, codec::StringType::Plain, false);
    encoder.pack(this->regexp, codec::StringType::Plain, false);
    encoder.pack(this->replacement, codec::StringType::Domain, false);
}

void NAPTR::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->order);
    decoder.unpack(this->preference);
    decoder.unpack(this->flags, codec::StringType::Plain);
    decoder.unpack(this->services, codec::StringType::Plain);
    decoder.unpack(this->regexp, codec::StringType::Plain);
    decoder.unpack(this->replacement, codec::StringType::Domain);
}

void NAPTR::unpack(const chen::json::object &object)
{
    map::write(object, "order", this->order);
    map::write(object, "preference", this->preference);
    map::write(object, "flags", this->flags);
    map::write(object, "services", this->services);
    map::write(object, "regexp", this->regexp);
    map::write(object, "replacement", this->replacement);
}


// -----------------------------------------------------------------------------
// KX
KX::KX() : RR(chen::dns::RRType::KX)
{
}

std::string KX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->exchanger;

    return ret;
}

std::shared_ptr<chen::dns::RR> KX::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void KX::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchanger, codec::StringType::Domain, false);
}

void KX::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->exchanger, codec::StringType::Domain);
}

void KX::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "exchanger", this->exchanger);
}


// -----------------------------------------------------------------------------
// CERT
CERT::CERT() : RR(chen::dns::RRType::CERT)
{
}

std::string CERT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->type);
    ret += " " + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::RR> CERT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CERT::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->type);
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void CERT::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->type);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void CERT::unpack(const chen::json::object &object)
{
    map::write(object, "type", this->type);
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// A6
A6::A6() : RR(chen::dns::RRType::A6)
{
}

std::string A6::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->prefix);
    ret += " " + RR::escape(this->suffix.size());
    ret += " " + this->prefix_name;

    return ret;
}

std::shared_ptr<chen::dns::RR> A6::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void A6::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->prefix);
    encoder.pack(this->suffix, this->suffix.size());
    encoder.pack(this->prefix_name, codec::StringType::Domain, false);
}

void A6::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->prefix);

    this->suffix.clear();
    decoder.unpack(this->suffix, static_cast<std::size_t>(128 - this->prefix));

    decoder.unpack(this->prefix_name, codec::StringType::Domain);
}

void A6::unpack(const chen::json::object &object)
{
    map::write(object, "prefix", this->prefix);

    this->suffix.clear();

    std::string suffix = map::find(object, "suffix", std::string());
    std::copy(suffix.begin(), suffix.end(), this->suffix.begin());

    map::write(object, "prefix_name", this->prefix_name);
}


// -----------------------------------------------------------------------------
// DNAME
DNAME::DNAME() : RR(chen::dns::RRType::DNAME)
{
}

std::string DNAME::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->target;
    return ret;
}

std::shared_ptr<chen::dns::RR> DNAME::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DNAME::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->target, codec::StringType::Domain, false);
}

void DNAME::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->target, codec::StringType::Domain);
}

void DNAME::unpack(const chen::json::object &object)
{
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// SINK
SINK::SINK() : RR(chen::dns::RRType::SINK)
{
}

std::string SINK::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->coding);
    ret += " " + num::str(this->subcoding);
    ret += " " + RR::escape(this->sdata.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> SINK::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SINK::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->coding);
    encoder.pack(this->subcoding);
    encoder.pack(this->sdata, this->sdata.size());
}

void SINK::unpack(chen::dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->coding);
    decoder.unpack(this->subcoding);

    this->sdata.clear();
    decoder.unpack(this->sdata, this->remain(std::distance(tmp, decoder.cur())));
}

void SINK::unpack(const chen::json::object &object)
{
    map::write(object, "coding", this->coding);
    map::write(object, "subcoding", this->subcoding);

    this->sdata.clear();

    std::string sdata = map::find(object, "sdata", std::string());
    std::copy(sdata.begin(), sdata.end(), this->sdata.begin());
}


// -----------------------------------------------------------------------------
// OPT
OPT::OPT() : RR(chen::dns::RRType::OPT)
{
    // opt name is always '.'
    this->name = ".";
}

// property
std::uint16_t OPT::payload() const
{
    return static_cast<std::uint16_t>(this->rrclass);
}

std::uint16_t OPT::rcode(chen::dns::RCODE code) const
{
    std::uint16_t ret = this->rcode();
    return ret << 4 | static_cast<std::uint8_t>(code);
}

std::uint8_t OPT::rcode() const
{
    return static_cast<std::uint8_t>(this->ttl >> 24 & 0xFF);
}

std::uint8_t OPT::version() const
{
    return static_cast<std::uint8_t>(this->ttl >> 16 & 0xFF);
}

std::uint16_t OPT::flag() const
{
    return static_cast<std::uint16_t>(this->ttl & 0xFFFF);
}

bool OPT::isDO() const
{
    return (this->flag() & 0x8000) == 0x8000;
}

std::uint16_t OPT::z() const
{
    return static_cast<std::uint16_t>(this->flag() & 0x7FFF);
}

void OPT::setPayload(std::uint16_t value)
{
    this->rrclass = static_cast<chen::dns::RRClass>(value);
}

void OPT::setRcode(std::uint16_t value)
{
    this->setRcode(static_cast<std::uint8_t>(value >> 4 & 0xFF));
}

void OPT::setRcode(std::uint8_t value)
{
    this->ttl = (this->ttl & 0x00FFFFFF) | (static_cast<std::uint32_t>(value) << 24);
}

void OPT::setVersion(std::uint8_t value)
{
    this->ttl = (this->ttl & 0xFF00FFFF) | (static_cast<std::uint32_t>(value) << 16);
}

void OPT::setFlag(std::uint16_t value)
{
    this->ttl = (this->ttl & 0xFFFF0000) | value;
}

void OPT::setDO(bool value)
{
    this->ttl &= 0xFFFF7FFF;

    if (value)
        this->ttl |= 0x8000;
}

void OPT::setZ(std::uint16_t value)
{
    this->ttl = (this->ttl & 0x8000) | value;
}

// override
std::string OPT::str(const std::string &sep) const
{
    // OPT str is special than other records
    std::string ret("OPT edns");

    ret += num::str(this->version()) + sep;
    ret += "flag" + sep;
    ret += str::format("0x%04X", this->flag()) + sep;
    ret += "udp" + sep;
    ret += num::str(this->payload()) + sep;
    ret += "option" + sep;
    ret += num::str(this->options.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> OPT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void OPT::pack(chen::dns::encoder &encoder) const
{
    for (auto &option : this->options)
        option->encode(encoder);
}

void OPT::unpack(chen::dns::decoder &decoder)
{
    this->options.clear();

    while (decoder.cur() != decoder.end())
        this->options.emplace_back(edns0::Option::create(decoder));
}

void OPT::unpack(const chen::json::object &object)
{
    throw error_codec("dns: unpack opt record by json is invalid");
}


// -----------------------------------------------------------------------------
// DS
DS::DS() : RR(chen::dns::RRType::DS)
{
}

std::string DS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> DS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DS::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void DS::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void DS::unpack(const chen::json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// SSHFP
SSHFP::SSHFP() : RR(chen::dns::RRType::SSHFP)
{
}

std::string SSHFP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->algorithm);
    ret += " " + num::str(this->fptype);
    ret += " " + this->fingerprint;

    return ret;
}

std::shared_ptr<chen::dns::RR> SSHFP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SSHFP::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->algorithm);
    encoder.pack(this->fptype);
    encoder.pack(this->fingerprint, codec::StringType::Plain, false);
}

void SSHFP::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->algorithm);
    decoder.unpack(this->fptype);
    decoder.unpack(this->fingerprint, codec::StringType::Plain);
}

void SSHFP::unpack(const chen::json::object &object)
{
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "fptype", this->fptype);
    map::write(object, "fingerprint", this->fingerprint);
}


// -----------------------------------------------------------------------------
// IPSECKEY
IPSECKEY::IPSECKEY() : RR(chen::dns::RRType::IPSECKEY)
{
}

std::string IPSECKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->precedence);
    ret += " " + num::str(this->gateway_type);
    ret += " " + num::str(this->algorithm);
    ret += " " + RR::escape(this->gateway.size());
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> IPSECKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void IPSECKEY::pack(chen::dns::encoder &encoder) const
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
            encoder.pack(domain, codec::StringType::Domain, false);
        }
            break;
    }

    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void IPSECKEY::unpack(chen::dns::decoder &decoder)
{
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
            decoder.unpack(domain, codec::StringType::Domain);

            this->gateway.insert(this->gateway.begin(), domain.begin(), domain.end());
        }
            break;
    }

    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void IPSECKEY::unpack(const chen::json::object &object)
{
    map::write(object, "precedence", this->precedence);
    map::write(object, "gateway_type", this->gateway_type);
    map::write(object, "algorithm", this->algorithm);

    this->gateway.clear();

    std::string gateway = map::find(object, "gateway", std::string());
    std::copy(gateway.begin(), gateway.end(), this->gateway.begin());

    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// RRSIG
RRSIG::RRSIG() : RR(chen::dns::RRType::RRSIG)
{
}

std::string RRSIG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->type_covered);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->labels);
    ret += " " + num::str(this->original);
    ret += " " + num::str(this->expiration);
    ret += " " + num::str(this->inception);
    ret += " " + num::str(this->key_tag);
    ret += " " + this->signer;
    ret += " " + this->signature;

    return ret;
}

std::shared_ptr<chen::dns::RR> RRSIG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void RRSIG::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->type_covered);
    encoder.pack(this->algorithm);
    encoder.pack(this->labels);
    encoder.pack(this->original);
    encoder.pack(this->expiration);
    encoder.pack(this->inception);
    encoder.pack(this->key_tag);
    encoder.pack(this->signer, codec::StringType::Domain, false);
    encoder.pack(this->signature, codec::StringType::Plain, false);
}

void RRSIG::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->type_covered);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->labels);
    decoder.unpack(this->original);
    decoder.unpack(this->expiration);
    decoder.unpack(this->inception);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->signer, codec::StringType::Domain);
    decoder.unpack(this->signature, codec::StringType::Plain);
}

void RRSIG::unpack(const chen::json::object &object)
{
    map::write(object, "type_covered", this->type_covered);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "labels", this->labels);
    map::write(object, "original", this->original);
    map::write(object, "expiration", this->expiration);
    map::write(object, "inception", this->inception);
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "signer", this->signer);
    map::write(object, "signature", this->signature);
}


// -----------------------------------------------------------------------------
// NSEC
NSEC::NSEC() : RR(chen::dns::RRType::NSEC)
{
}

std::string NSEC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->next_domain;
    ret += " " + RR::escape(this->type_bitmap.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> NSEC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NSEC::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, codec::StringType::Domain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void NSEC::unpack(chen::dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->next_domain, codec::StringType::Domain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void NSEC::unpack(const chen::json::object &object)
{
    map::write(object, "next_domain", this->next_domain);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// DNSKEY
DNSKEY::DNSKEY() : RR(chen::dns::RRType::DNSKEY)
{
}

std::string DNSKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> DNSKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DNSKEY::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void DNSKEY::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void DNSKEY::unpack(const chen::json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// DHCID
DHCID::DHCID() : RR(chen::dns::RRType::DHCID)
{
}

std::string DHCID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->digest;
    return ret;
}

std::shared_ptr<chen::dns::RR> DHCID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DHCID::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void DHCID::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void DHCID::unpack(const chen::json::object &object)
{
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// NSEC3
NSEC3::NSEC3() : RR(chen::dns::RRType::NSEC3)
{
}

std::string NSEC3::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->hash);
    ret += " " + num::str(this->flags);
    ret += " " + num::str(this->iterations);
    ret += " " + num::str(this->salt_length);
    ret += " " + RR::escape(this->salt.size());
    ret += " " + num::str(this->hash_length);
    ret += " " + this->next_owner;
    ret += " " + RR::escape(this->type_bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> NSEC3::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NSEC3::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->hash);
    encoder.pack(this->flags);
    encoder.pack(this->iterations);
    encoder.pack(this->salt_length);
    encoder.pack(this->salt, this->salt_length);
    encoder.pack(this->hash_length);
    encoder.pack(this->next_owner, codec::StringType::Plain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void NSEC3::unpack(chen::dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->hash);
    decoder.unpack(this->flags);
    decoder.unpack(this->iterations);
    decoder.unpack(this->salt_length);
    decoder.unpack(this->salt, this->salt_length);
    decoder.unpack(this->hash_length);
    decoder.unpack(this->next_owner, codec::StringType::Plain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void NSEC3::unpack(const chen::json::object &object)
{
    map::write(object, "hash", this->hash);
    map::write(object, "flags", this->flags);
    map::write(object, "iterations", this->iterations);
    map::write(object, "salt_length", this->salt_length);

    this->salt.clear();

    std::string salt = map::find(object, "salt", std::string());
    std::copy(salt.begin(), salt.end(), this->salt.begin());

    map::write(object, "hash_length", this->hash_length);
    map::write(object, "next_owner", this->next_owner);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// NSEC3PARAM
NSEC3PARAM::NSEC3PARAM() : RR(chen::dns::RRType::NSEC3PARAM)
{
}

std::string NSEC3PARAM::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->hash);
    ret += " " + num::str(this->flags);
    ret += " " + num::str(this->iterations);
    ret += " " + num::str(this->salt_length);
    ret += " " + RR::escape(this->salt.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> NSEC3PARAM::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NSEC3PARAM::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->hash);
    encoder.pack(this->flags);
    encoder.pack(this->iterations);
    encoder.pack(this->salt_length);
    encoder.pack(this->salt, this->salt_length);
}

void NSEC3PARAM::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->hash);
    decoder.unpack(this->flags);
    decoder.unpack(this->iterations);
    decoder.unpack(this->salt_length);
    decoder.unpack(this->salt, this->salt_length);
}

void NSEC3PARAM::unpack(const chen::json::object &object)
{
    map::write(object, "hash", this->hash);
    map::write(object, "flags", this->flags);
    map::write(object, "iterations", this->iterations);
    map::write(object, "salt_length", this->salt_length);

    this->salt.clear();

    std::string salt = map::find(object, "salt", std::string());
    std::copy(salt.begin(), salt.end(), this->salt.begin());
}


// -----------------------------------------------------------------------------
// TLSA
TLSA::TLSA() : RR(chen::dns::RRType::TLSA)
{
}

std::string TLSA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->usage);
    ret += " " + num::str(this->selector);
    ret += " " + num::str(this->matching_type);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::RR> TLSA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void TLSA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void TLSA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void TLSA::unpack(const chen::json::object &object)
{
    map::write(object, "usage", this->usage);
    map::write(object, "selector", this->selector);
    map::write(object, "matching_type", this->matching_type);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// SMIMEA
SMIMEA::SMIMEA() : RR(chen::dns::RRType::SMIMEA)
{
}

std::string SMIMEA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->usage);
    ret += " " + num::str(this->selector);
    ret += " " + num::str(this->matching_type);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::RR> SMIMEA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SMIMEA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void SMIMEA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void SMIMEA::unpack(const chen::json::object &object)
{
    map::write(object, "usage", this->usage);
    map::write(object, "selector", this->selector);
    map::write(object, "matching_type", this->matching_type);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// HIP
HIP::HIP() : RR(chen::dns::RRType::HIP)
{
}

std::string HIP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->hit_length);
    ret += " " + num::str(this->pk_algorithm);
    ret += " " + num::str(this->pk_length);
    ret += " " + this->hit;
    ret += " " + this->publickey;
    ret += " " + this->rendezvous_servers;

    return ret;
}

std::shared_ptr<chen::dns::RR> HIP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void HIP::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->hit_length);
    encoder.pack(this->pk_algorithm);
    encoder.pack(this->pk_length);
    encoder.pack(this->hit, codec::StringType::Plain, false);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
    encoder.pack(this->rendezvous_servers, codec::StringType::Plain, false);
}

void HIP::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->hit_length);
    decoder.unpack(this->pk_algorithm);
    decoder.unpack(this->pk_length);
    decoder.unpack(this->hit, codec::StringType::Plain);
    decoder.unpack(this->publickey, codec::StringType::Plain);
    decoder.unpack(this->rendezvous_servers, codec::StringType::Plain);
}

void HIP::unpack(const chen::json::object &object)
{
    map::write(object, "hit_length", this->hit_length);
    map::write(object, "pk_algorithm", this->pk_algorithm);
    map::write(object, "pk_length", this->pk_length);
    map::write(object, "hit", this->hit);
    map::write(object, "publickey", this->publickey);
    map::write(object, "rendezvous_servers", this->rendezvous_servers);
}


// -----------------------------------------------------------------------------
// NINFO
NINFO::NINFO() : RR(chen::dns::RRType::NINFO)
{
}

std::string NINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->zs_data;
    return ret;
}

std::shared_ptr<chen::dns::RR> NINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NINFO::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->zs_data, codec::StringType::Plain, false);
}

void NINFO::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->zs_data, codec::StringType::Plain);
}

void NINFO::unpack(const chen::json::object &object)
{
    map::write(object, "zs_data", this->zs_data);
}


// -----------------------------------------------------------------------------
// RKEY
RKEY::RKEY() : RR(chen::dns::RRType::RKEY)
{
}

std::string RKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> RKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void RKEY::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void RKEY::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void RKEY::unpack(const chen::json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// TALINK
TALINK::TALINK() : RR(chen::dns::RRType::TALINK)
{
}

std::string TALINK::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->previous_name;
    ret += " " + this->next_name;

    return ret;
}

std::shared_ptr<chen::dns::RR> TALINK::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void TALINK::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->previous_name, codec::StringType::Domain, false);
    encoder.pack(this->next_name, codec::StringType::Domain, false);
}

void TALINK::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->previous_name, codec::StringType::Domain);
    decoder.unpack(this->next_name, codec::StringType::Domain);
}

void TALINK::unpack(const chen::json::object &object)
{
    map::write(object, "previous_name", this->previous_name);
    map::write(object, "next_name", this->next_name);
}


// -----------------------------------------------------------------------------
// CDS
CDS::CDS() : RR(chen::dns::RRType::CDS)
{
}

std::string CDS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> CDS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CDS::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void CDS::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void CDS::unpack(const chen::json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// CDNSKEY
CDNSKEY::CDNSKEY() : RR(chen::dns::RRType::CDNSKEY)
{
}

std::string CDNSKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> CDNSKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CDNSKEY::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void CDNSKEY::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void CDNSKEY::unpack(const chen::json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
OPENPGPKEY::OPENPGPKEY() : RR(chen::dns::RRType::OPENPGPKEY)
{
}

std::string OPENPGPKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->publickey;
    return ret;
}

std::shared_ptr<chen::dns::RR> OPENPGPKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void OPENPGPKEY::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void OPENPGPKEY::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void OPENPGPKEY::unpack(const chen::json::object &object)
{
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// CSYNC
CSYNC::CSYNC() : RR(chen::dns::RRType::CSYNC)
{
}

std::string CSYNC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->serial);
    ret += " " + num::str(this->flags);
    ret += " " + RR::escape(this->type_bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> CSYNC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CSYNC::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->serial);
    encoder.pack(this->flags);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void CSYNC::unpack(chen::dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->serial);
    decoder.unpack(this->flags);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void CSYNC::unpack(const chen::json::object &object)
{
    map::write(object, "serial", this->serial);
    map::write(object, "flags", this->flags);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// SPF
SPF::SPF() : RR(chen::dns::RRType::SPF)
{
}

std::string SPF::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->txt);
    return ret;
}

std::shared_ptr<chen::dns::RR> SPF::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void SPF::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->txt, codec::StringType::Plain, false);
}

void SPF::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->txt, codec::StringType::Plain);
}

void SPF::unpack(const chen::json::object &object)
{
    map::write(object, "txt", this->txt);
}


// -----------------------------------------------------------------------------
// UINFO
UINFO::UINFO() : Raw(chen::dns::RRType::UINFO)
{
}

std::shared_ptr<chen::dns::RR> UINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// UID
UID::UID() : Raw(chen::dns::RRType::UID)
{
}

std::shared_ptr<chen::dns::RR> UID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// GID
GID::GID() : Raw(chen::dns::RRType::GID)
{
}

std::shared_ptr<chen::dns::RR> GID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// UNSPEC
UNSPEC::UNSPEC() : Raw(chen::dns::RRType::UNSPEC)
{
}

std::shared_ptr<chen::dns::RR> UNSPEC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// NID
NID::NID() : RR(chen::dns::RRType::NID)
{
}

std::string NID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->node_id);

    return ret;
}

std::shared_ptr<chen::dns::RR> NID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void NID::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->node_id);
}

void NID::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->node_id);
}

void NID::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "node_id", this->node_id);
}


// -----------------------------------------------------------------------------
// L32
L32::L32() : RR(chen::dns::RRType::L32)
{
}

std::string L32::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->locator32);

    return ret;
}

std::shared_ptr<chen::dns::RR> L32::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void L32::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator32);
}

void L32::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->locator32);
}

void L32::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "locator32", this->locator32);
}


// -----------------------------------------------------------------------------
// L64
L64::L64() : RR(chen::dns::RRType::L64)
{
}

std::string L64::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->locator64);

    return ret;
}

std::shared_ptr<chen::dns::RR> L64::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void L64::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator64);
}

void L64::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->locator64);
}

void L64::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "locator64", this->locator64);
}


// -----------------------------------------------------------------------------
// LP
LP::LP() : RR(chen::dns::RRType::LP)
{
}

std::string LP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->fqdn;

    return ret;
}

std::shared_ptr<chen::dns::RR> LP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void LP::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->fqdn, codec::StringType::Domain, false);
}

void LP::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->fqdn, codec::StringType::Domain);
}

void LP::unpack(const chen::json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "fqdn", this->fqdn);
}


// -----------------------------------------------------------------------------
// EUI48
EUI48::EUI48() : RR(chen::dns::RRType::EUI48)
{
}

std::string EUI48::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->address.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> EUI48::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void EUI48::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void EUI48::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void EUI48::unpack(const chen::json::object &object)
{
    std::string address = map::find(object, "address", std::string());
    std::copy(address.begin(), address.end(), this->address.begin());
}


// -----------------------------------------------------------------------------
// EUI64
EUI64::EUI64() : RR(chen::dns::RRType::EUI64)
{
}

std::string EUI64::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->address);
    return ret;
}

std::shared_ptr<chen::dns::RR> EUI64::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void EUI64::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void EUI64::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void EUI64::unpack(const chen::json::object &object)
{
    map::write(object, "address", this->address);
}


// -----------------------------------------------------------------------------
// TKEY
TKEY::TKEY() : RR(chen::dns::RRType::TKEY)
{
}

std::string TKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->algorithm;
    ret += " " + num::str(this->inception);
    ret += " " + num::str(this->expiration);
    ret += " " + num::str(this->mode);
    ret += " " + num::str(this->error);
    ret += " " + num::str(this->key_size);
    ret += " " + RR::escape(this->key.size());
    ret += " " + num::str(this->other_len);
    ret += " " + RR::escape(this->other_data.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> TKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void TKEY::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->algorithm, codec::StringType::Domain, false);
    encoder.pack(this->inception);
    encoder.pack(this->expiration);
    encoder.pack(this->mode);
    encoder.pack(this->error);
    encoder.pack(this->key_size);
    encoder.pack(this->key, this->key_size);
    encoder.pack(this->other_len);
    encoder.pack(this->other_data, this->other_len);
}

void TKEY::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->algorithm, codec::StringType::Domain);
    decoder.unpack(this->inception);
    decoder.unpack(this->expiration);
    decoder.unpack(this->mode);
    decoder.unpack(this->error);
    decoder.unpack(this->key_size);
    decoder.unpack(this->key, this->key_size);
    decoder.unpack(this->other_len);
    decoder.unpack(this->other_data, this->other_len);
}

void TKEY::unpack(const chen::json::object &object)
{
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "inception", this->inception);
    map::write(object, "expiration", this->expiration);
    map::write(object, "mode", this->mode);
    map::write(object, "error", this->error);
    map::write(object, "key_size", this->key_size);

    this->key.clear();

    std::string key = map::find(object, "key", std::string());
    std::copy(key.begin(), key.end(), this->key.begin());

    map::write(object, "other_len", this->other_len);

    this->other_data.clear();

    std::string other_data = map::find(object, "other_data", std::string());
    std::copy(other_data.begin(), other_data.end(), this->other_data.begin());
}


// -----------------------------------------------------------------------------
// TSIG
TSIG::TSIG() : RR(chen::dns::RRType::TSIG)
{
}

std::string TSIG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->algorithm;
    ret += " " + RR::escape(this->time_signed.size());
    ret += " " + num::str(this->fudge);
    ret += " " + num::str(this->mac_size);
    ret += " " + RR::escape(this->mac.size());
    ret += " " + num::str(this->original_id);
    ret += " " + num::str(this->error);
    ret += " " + num::str(this->other_len);
    ret += " " + RR::escape(this->other_data.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> TSIG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void TSIG::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->algorithm, codec::StringType::Domain, false);
    encoder.pack(this->time_signed);
    encoder.pack(this->fudge);
    encoder.pack(this->mac_size);
    encoder.pack(this->mac, this->mac_size);
    encoder.pack(this->original_id);
    encoder.pack(this->error);
    encoder.pack(this->other_len);
    encoder.pack(this->other_data, this->other_len);
}

void TSIG::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->algorithm, codec::StringType::Domain);
    decoder.unpack(this->time_signed);
    decoder.unpack(this->fudge);
    decoder.unpack(this->mac_size);
    decoder.unpack(this->mac, this->mac_size);
    decoder.unpack(this->original_id);
    decoder.unpack(this->error);
    decoder.unpack(this->other_len);
    decoder.unpack(this->other_data, this->other_len);
}

void TSIG::unpack(const chen::json::object &object)
{
    map::write(object, "algorithm", this->algorithm);

    std::string time_signed = map::find(object, "time_signed", std::string());
    std::copy(time_signed.begin(), time_signed.end(), this->time_signed.begin());

    map::write(object, "fudge", this->fudge);
    map::write(object, "mac_size", this->mac_size);

    this->mac.clear();

    std::string mac = map::find(object, "mac", std::string());
    std::copy(mac.begin(), mac.end(), this->mac.begin());

    map::write(object, "original_id", this->original_id);
    map::write(object, "error", this->error);
    map::write(object, "other_len", this->other_len);

    this->other_data.clear();

    std::string other_data = map::find(object, "other_data", std::string());
    std::copy(other_data.begin(), other_data.end(), this->other_data.begin());
}


// -----------------------------------------------------------------------------
// URI
URI::URI() : RR(chen::dns::RRType::URI)
{
}

std::string URI::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->priority);
    ret += " " + num::str(this->weight);
    ret += " " + this->target;

    return ret;
}

std::shared_ptr<chen::dns::RR> URI::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void URI::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->target, codec::StringType::Plain, false);
}

void URI::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->target, codec::StringType::Plain);
}

void URI::unpack(const chen::json::object &object)
{
    map::write(object, "priority", this->priority);
    map::write(object, "weight", this->weight);
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// CAA
CAA::CAA() : RR(chen::dns::RRType::CAA)
{
}

std::string CAA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + this->tag;
    ret += " " + this->value;

    return ret;
}

std::shared_ptr<chen::dns::RR> CAA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void CAA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->tag, codec::StringType::Plain, false);
    encoder.pack(this->value, codec::StringType::Plain, false);
}

void CAA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->tag, codec::StringType::Plain);
    decoder.unpack(this->value, codec::StringType::Plain);
}

void CAA::unpack(const chen::json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "tag", this->tag);
    map::write(object, "value", this->value);
}


// -----------------------------------------------------------------------------
// TA
TA::TA() : RR(chen::dns::RRType::TA)
{
}

std::string TA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> TA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void TA::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void TA::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void TA::unpack(const chen::json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// DLV
DLV::DLV() : RR(chen::dns::RRType::DLV)
{
}

std::string DLV::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> DLV::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void DLV::pack(chen::dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void DLV::unpack(chen::dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void DLV::unpack(const chen::json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}