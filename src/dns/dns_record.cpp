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
#include <cctype>

// -----------------------------------------------------------------------------
// RR
chen::dns::RR::RR(RRType type) : rrtype(type)
{
}

// encode & decode
void chen::dns::RR::encode(dns::encoder &encoder) const
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

    if (off > (std::numeric_limits<std::uint16_t>::max)())
        throw std::runtime_error("dns: codec pack rdata size is overflow");

    auto tmp = static_cast<std::uint16_t>(off);

    encoder.change(len - tmp - 2, static_cast<std::uint8_t>(tmp >> 8 & 0xFF));
    encoder.change(len - tmp - 1, static_cast<std::uint8_t>(tmp & 0xFF));
}

void chen::dns::RR::decode(dns::decoder &decoder)
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

void chen::dns::RR::decode(const json::object &object)
{
    // unpack base fields, ignore rdlength
    map::write(object, "name", this->name);
    map::write(object, "rrtype", reinterpret_cast<std::uint16_t&>(this->rrtype));
    map::write(object, "rrclass", reinterpret_cast<std::uint16_t&>(this->rrclass));
    map::write(object, "ttl", this->ttl);

    // unpack subclass
    this->unpack(object);
}

std::shared_ptr<chen::dns::RR> chen::dns::RR::create(dns::decoder &decoder)
{
    dns::decoder detect(decoder);

    // detect type
    std::string unused;
    detect.unpack(unused, codec::StringType::Domain);

    RRType rrtype = RRType::None;
    detect.unpack(rrtype);

    // build record
    std::shared_ptr<RR> record = table::build(rrtype);
    if (!record)
        return nullptr;

    // decode it
    record->decode(decoder);

    return record;
}

// str
std::string chen::dns::RR::str(const std::string &sep) const
{
    std::string ret;

    ret += this->name + sep;
    ret += num::str(this->ttl) + sep;
    ret += table::classToText(this->rrclass) + sep;
    ret += table::typeToText(this->rrtype);

    return ret;
}

// helper
std::size_t chen::dns::RR::remain(std::size_t used) const
{
    if (this->rdlength < used)
        throw std::runtime_error("dns: codec rdata is overflow");

    return static_cast<std::size_t>(this->rdlength - used);
}

std::string chen::dns::RR::escape(const std::vector<std::uint8_t> &data)
{
    std::string ret;

    for (auto ch : data)
        ret += str::format("%02X", ch);

    return ret;
}

std::string chen::dns::RR::escape(const std::string &text)
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

std::string chen::dns::RR::escape(std::size_t bits)
{
    return "<<" + num::str(bits) + "bits>>";
}


// -----------------------------------------------------------------------------
// Raw
chen::dns::Raw::Raw() : Raw(RRType::None)
{
}

chen::dns::Raw::Raw(RRType type) : RR(type)
{
}

std::string chen::dns::Raw::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->rdata.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::Raw::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::Raw::pack(dns::encoder &encoder) const
{
    encoder.pack(this->rdata, this->rdata.size());
}

void chen::dns::Raw::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->rdata, this->rdlength);
}

void chen::dns::Raw::unpack(const json::object &object)
{
    this->rdata.clear();

    std::string rdata = map::find(object, "rdata");
    std::copy(rdata.begin(), rdata.end(), this->rdata.begin());
}


// -----------------------------------------------------------------------------
// A
chen::dns::A::A() : RR(RRType::A)
{
}

std::string chen::dns::A::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + ip::version4::toString(this->address);
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::A::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::A::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::A::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::A::unpack(const json::object &object)
{
    // if address is string then use address_v4::toInteger
    auto address = map::find(object, "address");

    if (address.isString())
        this->address = ip::version4::toInteger(address);
    else
        this->address = address.toUnsigned();
}


// -----------------------------------------------------------------------------
// NS
chen::dns::NS::NS() : RR(RRType::NS)
{
}

std::string chen::dns::NS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->nsdname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NS::pack(dns::encoder &encoder) const
{
    encoder.pack(this->nsdname, codec::StringType::Domain, false);
}

void chen::dns::NS::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->nsdname, codec::StringType::Domain);
}

void chen::dns::NS::unpack(const json::object &object)
{
    map::write(object, "nsdname", this->nsdname);
}


// -----------------------------------------------------------------------------
// MD
chen::dns::MD::MD() : RR(RRType::MD)
{
}

std::string chen::dns::MD::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->madname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MD::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MD::pack(dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void chen::dns::MD::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void chen::dns::MD::unpack(const json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// MF
chen::dns::MF::MF() : RR(RRType::MF)
{
}

std::string chen::dns::MF::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += this->madname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MF::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MF::pack(dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void chen::dns::MF::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void chen::dns::MF::unpack(const json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// CNAME
chen::dns::CNAME::CNAME() : RR(RRType::CNAME)
{
}

std::string chen::dns::CNAME::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->cname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::CNAME::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::CNAME::pack(dns::encoder &encoder) const
{
    encoder.pack(this->cname, codec::StringType::Domain, false);
}

void chen::dns::CNAME::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->cname, codec::StringType::Domain);
}

void chen::dns::CNAME::unpack(const json::object &object)
{
    map::write(object, "cname", this->cname);
}


// -----------------------------------------------------------------------------
// SOA
chen::dns::SOA::SOA() : RR(RRType::SOA)
{
}

std::string chen::dns::SOA::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::SOA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SOA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->mname, codec::StringType::Domain, false);
    encoder.pack(this->rname, codec::StringType::Domain, false);
    encoder.pack(this->serial);
    encoder.pack(this->refresh);
    encoder.pack(this->retry);
    encoder.pack(this->expire);
    encoder.pack(this->minimum);
}

void chen::dns::SOA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->mname, codec::StringType::Domain);
    decoder.unpack(this->rname, codec::StringType::Domain);
    decoder.unpack(this->serial);
    decoder.unpack(this->refresh);
    decoder.unpack(this->retry);
    decoder.unpack(this->expire);
    decoder.unpack(this->minimum);
}

void chen::dns::SOA::unpack(const json::object &object)
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
chen::dns::MB::MB() : RR(RRType::MB)
{
}

std::string chen::dns::MB::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->madname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MB::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MB::pack(dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void chen::dns::MB::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void chen::dns::MB::unpack(const json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// MG
chen::dns::MG::MG() : RR(RRType::MG)
{
}

std::string chen::dns::MG::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->mgmname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MG::pack(dns::encoder &encoder) const
{
    encoder.pack(this->mgmname, codec::StringType::Domain, false);
}

void chen::dns::MG::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->mgmname, codec::StringType::Domain);
}

void chen::dns::MG::unpack(const json::object &object)
{
    map::write(object, "mgmname", this->mgmname);
}


// -----------------------------------------------------------------------------
// MR
chen::dns::MR::MR() : RR(RRType::MR)
{
}

std::string chen::dns::MR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->newname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MR::pack(dns::encoder &encoder) const
{
    encoder.pack(this->newname, codec::StringType::Domain, false);
}

void chen::dns::MR::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->newname, codec::StringType::Domain);
}

void chen::dns::MR::unpack(const json::object &object)
{
    map::write(object, "newname", this->newname);
}


// -----------------------------------------------------------------------------
// Null
chen::dns::Null::Null() : Raw(RRType::Null)
{
}

std::string chen::dns::Null::str(const std::string &sep) const
{
    auto ret = Raw::str(sep);
    ret += sep + RR::escape(this->anything.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::Null::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::Null::pack(dns::encoder &encoder) const
{
    // base
    Raw::pack(encoder);

    // self
    encoder.pack(this->anything, this->anything.size());
}

void chen::dns::Null::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->anything, this->rdlength);
}

void chen::dns::Null::unpack(const json::object &object)
{
    Raw::unpack(object);

    this->anything.clear();

    std::string anything = map::find(object, "anything", std::string());
    std::copy(anything.begin(), anything.end(), this->anything.begin());
}


// -----------------------------------------------------------------------------
// WKS
chen::dns::WKS::WKS() : RR(RRType::WKS)
{
}

std::string chen::dns::WKS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->address);
    ret += " " + num::str(this->protocol);
    ret += " " + RR::escape(this->bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::WKS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::WKS::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
    encoder.pack(this->protocol);
    encoder.pack(this->bitmap, this->bitmap.size());
}

void chen::dns::WKS::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->address);
    decoder.unpack(this->protocol);

    this->bitmap.clear();
    decoder.unpack(this->bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void chen::dns::WKS::unpack(const json::object &object)
{
    map::write(object, "address", this->address);
    map::write(object, "protocol", this->protocol);

    this->bitmap.clear();

    std::string bitmap = map::find(object, "bitmap", std::string());
    std::copy(bitmap.begin(), bitmap.end(), this->bitmap.begin());
}


// -----------------------------------------------------------------------------
// PTR
chen::dns::PTR::PTR() : RR(RRType::PTR)
{
}

std::string chen::dns::PTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->ptrdname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::PTR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::PTR::pack(dns::encoder &encoder) const
{
    encoder.pack(this->ptrdname, codec::StringType::Domain, false);
}

void chen::dns::PTR::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->ptrdname, codec::StringType::Domain);
}

void chen::dns::PTR::unpack(const json::object &object)
{
    map::write(object, "ptrdname", this->ptrdname);
}


// -----------------------------------------------------------------------------
// HINFO
chen::dns::HINFO::HINFO() : RR(RRType::HINFO)
{
}

std::string chen::dns::HINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->cpu;
    ret += " " + this->os;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::HINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::HINFO::pack(dns::encoder &encoder) const
{
    encoder.pack(this->cpu, codec::StringType::Plain, false);
    encoder.pack(this->os, codec::StringType::Plain, false);
}

void chen::dns::HINFO::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->cpu, codec::StringType::Plain);
    decoder.unpack(this->os, codec::StringType::Plain);
}

void chen::dns::HINFO::unpack(const json::object &object)
{
    map::write(object, "cpu", this->cpu);
    map::write(object, "os", this->os);
}


// -----------------------------------------------------------------------------
// MINFO
chen::dns::MINFO::MINFO() : RR(RRType::MINFO)
{
}

std::string chen::dns::MINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->rmailbx;
    ret += " " + this->emailbx;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MINFO::pack(dns::encoder &encoder) const
{
    encoder.pack(this->rmailbx, codec::StringType::Domain, false);
    encoder.pack(this->emailbx, codec::StringType::Domain, false);
}

void chen::dns::MINFO::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->rmailbx, codec::StringType::Domain);
    decoder.unpack(this->emailbx, codec::StringType::Domain);
}

void chen::dns::MINFO::unpack(const json::object &object)
{
    map::write(object, "rmailbx", this->rmailbx);
    map::write(object, "emailbx", this->emailbx);
}


// -----------------------------------------------------------------------------
// MX
chen::dns::MX::MX() : RR(RRType::MX)
{
}

std::string chen::dns::MX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->preference);
    ret += " " + this->exchange;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::MX::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::MX::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchange, codec::StringType::Domain, false);
}

void chen::dns::MX::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->exchange, codec::StringType::Domain);
}

void chen::dns::MX::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "exchange", this->exchange);
}


// -----------------------------------------------------------------------------
// TXT
chen::dns::TXT::TXT() : RR(RRType::TXT)
{
}

std::string chen::dns::TXT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->txt_data);
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::TXT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::TXT::pack(dns::encoder &encoder) const
{
    encoder.pack(this->txt_data, codec::StringType::Plain, false);
}

void chen::dns::TXT::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->txt_data, codec::StringType::Plain);
}

void chen::dns::TXT::unpack(const json::object &object)
{
    map::write(object, "txt_data", this->txt_data);
}


// -----------------------------------------------------------------------------
// RP
chen::dns::RP::RP() : RR(RRType::RP)
{
}

std::string chen::dns::RP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->mbox_dname;
    ret += " " + this->txt_dname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::RP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::RP::pack(dns::encoder &encoder) const
{
    encoder.pack(this->mbox_dname, codec::StringType::Domain, false);
    encoder.pack(this->txt_dname, codec::StringType::Domain, false);
}

void chen::dns::RP::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->mbox_dname, codec::StringType::Domain);
    decoder.unpack(this->txt_dname, codec::StringType::Domain);
}

void chen::dns::RP::unpack(const json::object &object)
{
    map::write(object, "mbox_dname", this->mbox_dname);
    map::write(object, "txt_dname", this->txt_dname);
}


// -----------------------------------------------------------------------------
// AFSDB
chen::dns::AFSDB::AFSDB() : RR(RRType::AFSDB)
{
}

std::string chen::dns::AFSDB::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->subtype);
    ret += " " + this->hostname;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::AFSDB::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::AFSDB::pack(dns::encoder &encoder) const
{
    encoder.pack(this->subtype);
    encoder.pack(this->hostname, codec::StringType::Domain, false);
}

void chen::dns::AFSDB::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->subtype);
    decoder.unpack(this->hostname, codec::StringType::Domain);
}

void chen::dns::AFSDB::unpack(const json::object &object)
{
    map::write(object, "subtype", this->subtype);
    map::write(object, "hostname", this->hostname);
}


// -----------------------------------------------------------------------------
// X25
chen::dns::X25::X25() : RR(RRType::X25)
{
}

std::string chen::dns::X25::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->psdn_address;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::X25::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::X25::pack(dns::encoder &encoder) const
{
    encoder.pack(this->psdn_address, codec::StringType::Plain, false);
}

void chen::dns::X25::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->psdn_address, codec::StringType::Plain);
}

void chen::dns::X25::unpack(const json::object &object)
{
    map::write(object, "psdn_address", this->psdn_address);
}


// -----------------------------------------------------------------------------
// ISDN
chen::dns::ISDN::ISDN() : RR(RRType::ISDN)
{
}

std::string chen::dns::ISDN::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->isdn_address;
    ret += " " + this->sa;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::ISDN::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::ISDN::pack(dns::encoder &encoder) const
{
    encoder.pack(this->isdn_address, codec::StringType::Plain, false);
    encoder.pack(this->sa, codec::StringType::Plain, false);
}

void chen::dns::ISDN::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->isdn_address, codec::StringType::Plain);
    decoder.unpack(this->sa, codec::StringType::Plain);
}

void chen::dns::ISDN::unpack(const json::object &object)
{
    map::write(object, "isdn_address", this->isdn_address);
    map::write(object, "sa", this->sa);
}


// -----------------------------------------------------------------------------
// RT
chen::dns::RT::RT() : RR(RRType::RT)
{
}

std::string chen::dns::RT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->preference);
    ret += " " + this->intermediate_host;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::RT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::RT::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->intermediate_host, codec::StringType::Domain, false);
}

void chen::dns::RT::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->intermediate_host, codec::StringType::Domain);
}

void chen::dns::RT::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "intermediate_host", this->intermediate_host);
}


// -----------------------------------------------------------------------------
// NSAP
chen::dns::NSAP::NSAP() : RR(RRType::NSAP)
{
}

std::string chen::dns::NSAP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->nsap;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NSAP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NSAP::pack(dns::encoder &encoder) const
{
    encoder.pack(this->nsap, codec::StringType::Plain, false);
}

void chen::dns::NSAP::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->nsap, codec::StringType::Plain);
}

void chen::dns::NSAP::unpack(const json::object &object)
{
    map::write(object, "nsap", this->nsap);
}


// -----------------------------------------------------------------------------
// NSAPPTR
chen::dns::NSAPPTR::NSAPPTR() : RR(RRType::NSAPPTR)
{
}

std::string chen::dns::NSAPPTR::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->owner;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NSAPPTR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NSAPPTR::pack(dns::encoder &encoder) const
{
    encoder.pack(this->owner, codec::StringType::Domain, false);
}

void chen::dns::NSAPPTR::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->owner, codec::StringType::Domain);
}

void chen::dns::NSAPPTR::unpack(const json::object &object)
{
    map::write(object, "owner", this->owner);
}


// -----------------------------------------------------------------------------
// SIG
chen::dns::SIG::SIG() : RR(RRType::SIG)
{
}

std::string chen::dns::SIG::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::SIG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SIG::pack(dns::encoder &encoder) const
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

void chen::dns::SIG::unpack(dns::decoder &decoder)
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

void chen::dns::SIG::unpack(const json::object &object)
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
chen::dns::KEY::KEY() : RR(RRType::KEY)
{
}

std::string chen::dns::KEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::KEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::KEY::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::KEY::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::KEY::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// PX
chen::dns::PX::PX() : RR(RRType::PX)
{
}

std::string chen::dns::PX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->map822;
    ret += " " + this->mapx400;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::PX::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::PX::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->map822, codec::StringType::Domain, false);
    encoder.pack(this->mapx400, codec::StringType::Domain, false);
}

void chen::dns::PX::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->map822, codec::StringType::Domain);
    decoder.unpack(this->mapx400, codec::StringType::Domain);
}

void chen::dns::PX::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "map822", this->map822);
    map::write(object, "mapx400", this->mapx400);
}


// -----------------------------------------------------------------------------
// GPOS
chen::dns::GPOS::GPOS() : RR(RRType::GPOS)
{
}

std::string chen::dns::GPOS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->longitude;
    ret += " " + this->latitude;
    ret += " " + this->altitude;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::GPOS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::GPOS::pack(dns::encoder &encoder) const
{
    encoder.pack(this->longitude, codec::StringType::Plain, false);
    encoder.pack(this->latitude, codec::StringType::Plain, false);
    encoder.pack(this->altitude, codec::StringType::Plain, false);
}

void chen::dns::GPOS::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->longitude, codec::StringType::Plain);
    decoder.unpack(this->latitude, codec::StringType::Plain);
    decoder.unpack(this->altitude, codec::StringType::Plain);
}

void chen::dns::GPOS::unpack(const json::object &object)
{
    map::write(object, "longitude", this->longitude);
    map::write(object, "latitude", this->latitude);
    map::write(object, "altitude", this->altitude);
}


// -----------------------------------------------------------------------------
// AAAA
chen::dns::AAAA::AAAA() : RR(RRType::AAAA)
{
}

std::string chen::dns::AAAA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + ip::version6::toString(this->address.data());
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::AAAA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::AAAA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::AAAA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::AAAA::unpack(const json::object &object)
{
    auto address = map::find(object, "address");
    this->address = ip::version6::toBytes(address);
}


// -----------------------------------------------------------------------------
// LOC
chen::dns::LOC::LOC() : RR(RRType::LOC)
{
}

std::string chen::dns::LOC::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::LOC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::LOC::pack(dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->size);
    encoder.pack(this->horiz_pre);
    encoder.pack(this->vert_pre);
    encoder.pack(this->longitude);
    encoder.pack(this->latitude);
    encoder.pack(this->altitude);
}

void chen::dns::LOC::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->version);
    decoder.unpack(this->size);
    decoder.unpack(this->horiz_pre);
    decoder.unpack(this->vert_pre);
    decoder.unpack(this->longitude);
    decoder.unpack(this->latitude);
    decoder.unpack(this->altitude);
}

void chen::dns::LOC::unpack(const json::object &object)
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
chen::dns::NXT::NXT() : RR(RRType::NXT)
{
}

std::string chen::dns::NXT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->next_domain;
    ret += " " + RR::escape(this->type_bitmap.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NXT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NXT::pack(dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, codec::StringType::Domain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void chen::dns::NXT::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->next_domain, codec::StringType::Domain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void chen::dns::NXT::unpack(const json::object &object)
{
    map::write(object, "next_domain", this->next_domain);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// EID
chen::dns::EID::EID() : RR(RRType::EID)
{
}

std::string chen::dns::EID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->endpoint;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::EID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::EID::pack(dns::encoder &encoder) const
{
    encoder.pack(this->endpoint, codec::StringType::Plain, false);
}

void chen::dns::EID::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->endpoint, codec::StringType::Plain);
}

void chen::dns::EID::unpack(const json::object &object)
{
    map::write(object, "endpoint", this->endpoint);
}


// -----------------------------------------------------------------------------
// NIMLOC
chen::dns::NIMLOC::NIMLOC() : RR(RRType::NIMLOC)
{
}

std::string chen::dns::NIMLOC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->locator;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NIMLOC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NIMLOC::pack(dns::encoder &encoder) const
{
    encoder.pack(this->locator, codec::StringType::Plain, false);
}

void chen::dns::NIMLOC::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->locator, codec::StringType::Plain);
}

void chen::dns::NIMLOC::unpack(const json::object &object)
{
    map::write(object, "locator", this->locator);
}


// -----------------------------------------------------------------------------
// SRV
chen::dns::SRV::SRV() : RR(RRType::SRV)
{
}

std::string chen::dns::SRV::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->priority);
    ret += " " + num::str(this->weight);
    ret += " " + num::str(this->port);
    ret += " " + this->target;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::SRV::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SRV::pack(dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->port);
    encoder.pack(this->target, codec::StringType::Domain, false);
}

void chen::dns::SRV::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->port);
    decoder.unpack(this->target, codec::StringType::Domain);
}

void chen::dns::SRV::unpack(const json::object &object)
{
    map::write(object, "priority", this->priority);
    map::write(object, "weight", this->weight);
    map::write(object, "port", this->port);
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// ATMA
chen::dns::ATMA::ATMA() : RR(RRType::ATMA)
{
}

std::string chen::dns::ATMA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->format);
    ret += " " + this->address;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::ATMA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::ATMA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->format);
    encoder.pack(this->address, codec::StringType::Plain, false);
}

void chen::dns::ATMA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->format);
    decoder.unpack(this->address, codec::StringType::Plain);
}

void chen::dns::ATMA::unpack(const json::object &object)
{
    map::write(object, "format", this->format);
    map::write(object, "address", this->address);
}


// -----------------------------------------------------------------------------
// NAPTR
chen::dns::NAPTR::NAPTR() : RR(RRType::NAPTR)
{
}

std::string chen::dns::NAPTR::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::NAPTR::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NAPTR::pack(dns::encoder &encoder) const
{
    encoder.pack(this->order);
    encoder.pack(this->preference);
    encoder.pack(this->flags, codec::StringType::Plain, false);
    encoder.pack(this->services, codec::StringType::Plain, false);
    encoder.pack(this->regexp, codec::StringType::Plain, false);
    encoder.pack(this->replacement, codec::StringType::Domain, false);
}

void chen::dns::NAPTR::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->order);
    decoder.unpack(this->preference);
    decoder.unpack(this->flags, codec::StringType::Plain);
    decoder.unpack(this->services, codec::StringType::Plain);
    decoder.unpack(this->regexp, codec::StringType::Plain);
    decoder.unpack(this->replacement, codec::StringType::Domain);
}

void chen::dns::NAPTR::unpack(const json::object &object)
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
chen::dns::KX::KX() : RR(RRType::KX)
{
}

std::string chen::dns::KX::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->exchanger;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::KX::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::KX::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchanger, codec::StringType::Domain, false);
}

void chen::dns::KX::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->exchanger, codec::StringType::Domain);
}

void chen::dns::KX::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "exchanger", this->exchanger);
}


// -----------------------------------------------------------------------------
// CERT
chen::dns::CERT::CERT() : RR(RRType::CERT)
{
}

std::string chen::dns::CERT::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->type);
    ret += " " + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::CERT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::CERT::pack(dns::encoder &encoder) const
{
    encoder.pack(this->type);
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void chen::dns::CERT::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->type);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void chen::dns::CERT::unpack(const json::object &object)
{
    map::write(object, "type", this->type);
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// A6
chen::dns::A6::A6() : RR(RRType::A6)
{
}

std::string chen::dns::A6::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->prefix);
    ret += " " + RR::escape(this->suffix.size());
    ret += " " + this->prefix_name;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::A6::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::A6::pack(dns::encoder &encoder) const
{
    encoder.pack(this->prefix);
    encoder.pack(this->suffix, this->suffix.size());
    encoder.pack(this->prefix_name, codec::StringType::Domain, false);
}

void chen::dns::A6::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->prefix);

    this->suffix.clear();
    decoder.unpack(this->suffix, static_cast<std::size_t>(128 - this->prefix));

    decoder.unpack(this->prefix_name, codec::StringType::Domain);
}

void chen::dns::A6::unpack(const json::object &object)
{
    map::write(object, "prefix", this->prefix);

    this->suffix.clear();

    std::string suffix = map::find(object, "suffix", std::string());
    std::copy(suffix.begin(), suffix.end(), this->suffix.begin());

    map::write(object, "prefix_name", this->prefix_name);
}


// -----------------------------------------------------------------------------
// DNAME
chen::dns::DNAME::DNAME() : RR(RRType::DNAME)
{
}

std::string chen::dns::DNAME::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->target;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::DNAME::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::DNAME::pack(dns::encoder &encoder) const
{
    encoder.pack(this->target, codec::StringType::Domain, false);
}

void chen::dns::DNAME::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->target, codec::StringType::Domain);
}

void chen::dns::DNAME::unpack(const json::object &object)
{
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// SINK
chen::dns::SINK::SINK() : RR(RRType::SINK)
{
}

std::string chen::dns::SINK::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->coding);
    ret += " " + num::str(this->subcoding);
    ret += " " + RR::escape(this->sdata.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::SINK::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SINK::pack(dns::encoder &encoder) const
{
    encoder.pack(this->coding);
    encoder.pack(this->subcoding);
    encoder.pack(this->sdata, this->sdata.size());
}

void chen::dns::SINK::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->coding);
    decoder.unpack(this->subcoding);

    this->sdata.clear();
    decoder.unpack(this->sdata, this->remain(std::distance(tmp, decoder.cur())));
}

void chen::dns::SINK::unpack(const json::object &object)
{
    map::write(object, "coding", this->coding);
    map::write(object, "subcoding", this->subcoding);

    this->sdata.clear();

    std::string sdata = map::find(object, "sdata", std::string());
    std::copy(sdata.begin(), sdata.end(), this->sdata.begin());
}


// -----------------------------------------------------------------------------
// OPT
chen::dns::OPT::OPT() : RR(RRType::OPT)
{
    // opt name is always '.'
    this->name = ".";
}

// property
std::uint16_t chen::dns::OPT::payload() const
{
    return static_cast<std::uint16_t>(this->rrclass);
}

std::uint16_t chen::dns::OPT::rcode(RCode code) const
{
    std::uint16_t ret = this->rcode();
    return ret << 4 | static_cast<std::uint8_t>(code);
}

std::uint8_t chen::dns::OPT::rcode() const
{
    return static_cast<std::uint8_t>(this->ttl >> 24 & 0xFF);
}

std::uint8_t chen::dns::OPT::version() const
{
    return static_cast<std::uint8_t>(this->ttl >> 16 & 0xFF);
}

std::uint16_t chen::dns::OPT::flag() const
{
    return static_cast<std::uint16_t>(this->ttl & 0xFFFF);
}

bool chen::dns::OPT::dnssec() const
{
    return (this->flag() & 0x8000) == 0x8000;
}

std::uint16_t chen::dns::OPT::z() const
{
    return static_cast<std::uint16_t>(this->flag() & 0x7FFF);
}

void chen::dns::OPT::payload(std::uint16_t value)
{
    this->rrclass = static_cast<RRClass>(value);
}

void chen::dns::OPT::rcode(std::uint16_t value)
{
    this->rcode(static_cast<std::uint8_t>(value >> 4 & 0xFF));
}

void chen::dns::OPT::rcode(std::uint8_t value)
{
    this->ttl = (this->ttl & 0x00FFFFFF) | (static_cast<std::uint32_t>(value) << 24);
}

void chen::dns::OPT::version(std::uint8_t value)
{
    this->ttl = (this->ttl & 0xFF00FFFF) | (static_cast<std::uint32_t>(value) << 16);
}

void chen::dns::OPT::flag(std::uint16_t value)
{
    this->ttl = (this->ttl & 0xFFFF0000) | value;
}

void chen::dns::OPT::dnssec(bool value)
{
    this->ttl &= 0xFFFF7FFF;

    if (value)
        this->ttl |= 0x8000;
}

void chen::dns::OPT::z(std::uint16_t value)
{
    this->ttl = (this->ttl & 0x8000) | value;
}

// override
std::string chen::dns::OPT::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::OPT::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::OPT::pack(dns::encoder &encoder) const
{
    for (auto &option : this->options)
        option->encode(encoder);
}

void chen::dns::OPT::unpack(dns::decoder &decoder)
{
    this->options.clear();

    while (decoder.cur() != decoder.end())
        this->options.emplace_back(edns0::Option::create(decoder));
}

void chen::dns::OPT::unpack(const json::object &object)
{
    throw std::runtime_error("dns: unpack opt record by json is invalid");
}


// -----------------------------------------------------------------------------
// DS
chen::dns::DS::DS() : RR(RRType::DS)
{
}

std::string chen::dns::DS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::DS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::DS::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::DS::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::DS::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// SSHFP
chen::dns::SSHFP::SSHFP() : RR(RRType::SSHFP)
{
}

std::string chen::dns::SSHFP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->algorithm);
    ret += " " + num::str(this->fptype);
    ret += " " + this->fingerprint;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::SSHFP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SSHFP::pack(dns::encoder &encoder) const
{
    encoder.pack(this->algorithm);
    encoder.pack(this->fptype);
    encoder.pack(this->fingerprint, codec::StringType::Plain, false);
}

void chen::dns::SSHFP::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->algorithm);
    decoder.unpack(this->fptype);
    decoder.unpack(this->fingerprint, codec::StringType::Plain);
}

void chen::dns::SSHFP::unpack(const json::object &object)
{
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "fptype", this->fptype);
    map::write(object, "fingerprint", this->fingerprint);
}


// -----------------------------------------------------------------------------
// IPSECKEY
chen::dns::IPSECKEY::IPSECKEY() : RR(RRType::IPSECKEY)
{
}

std::string chen::dns::IPSECKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->precedence);
    ret += " " + num::str(this->gateway_type);
    ret += " " + num::str(this->algorithm);
    ret += " " + RR::escape(this->gateway.size());
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::IPSECKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::IPSECKEY::pack(dns::encoder &encoder) const
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

void chen::dns::IPSECKEY::unpack(dns::decoder &decoder)
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

void chen::dns::IPSECKEY::unpack(const json::object &object)
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
chen::dns::RRSIG::RRSIG() : RR(RRType::RRSIG)
{
}

std::string chen::dns::RRSIG::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::RRSIG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::RRSIG::pack(dns::encoder &encoder) const
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

void chen::dns::RRSIG::unpack(dns::decoder &decoder)
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

void chen::dns::RRSIG::unpack(const json::object &object)
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
chen::dns::NSEC::NSEC() : RR(RRType::NSEC)
{
}

std::string chen::dns::NSEC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->next_domain;
    ret += " " + RR::escape(this->type_bitmap.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NSEC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NSEC::pack(dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, codec::StringType::Domain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void chen::dns::NSEC::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->next_domain, codec::StringType::Domain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void chen::dns::NSEC::unpack(const json::object &object)
{
    map::write(object, "next_domain", this->next_domain);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// DNSKEY
chen::dns::DNSKEY::DNSKEY() : RR(RRType::DNSKEY)
{
}

std::string chen::dns::DNSKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::DNSKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::DNSKEY::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::DNSKEY::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::DNSKEY::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// DHCID
chen::dns::DHCID::DHCID() : RR(RRType::DHCID)
{
}

std::string chen::dns::DHCID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->digest;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::DHCID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::DHCID::pack(dns::encoder &encoder) const
{
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::DHCID::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::DHCID::unpack(const json::object &object)
{
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// NSEC3
chen::dns::NSEC3::NSEC3() : RR(RRType::NSEC3)
{
}

std::string chen::dns::NSEC3::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::NSEC3::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NSEC3::pack(dns::encoder &encoder) const
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

void chen::dns::NSEC3::unpack(dns::decoder &decoder)
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

void chen::dns::NSEC3::unpack(const json::object &object)
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
chen::dns::NSEC3PARAM::NSEC3PARAM() : RR(RRType::NSEC3PARAM)
{
}

std::string chen::dns::NSEC3PARAM::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->hash);
    ret += " " + num::str(this->flags);
    ret += " " + num::str(this->iterations);
    ret += " " + num::str(this->salt_length);
    ret += " " + RR::escape(this->salt.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NSEC3PARAM::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NSEC3PARAM::pack(dns::encoder &encoder) const
{
    encoder.pack(this->hash);
    encoder.pack(this->flags);
    encoder.pack(this->iterations);
    encoder.pack(this->salt_length);
    encoder.pack(this->salt, this->salt_length);
}

void chen::dns::NSEC3PARAM::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->hash);
    decoder.unpack(this->flags);
    decoder.unpack(this->iterations);
    decoder.unpack(this->salt_length);
    decoder.unpack(this->salt, this->salt_length);
}

void chen::dns::NSEC3PARAM::unpack(const json::object &object)
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
chen::dns::TLSA::TLSA() : RR(RRType::TLSA)
{
}

std::string chen::dns::TLSA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->usage);
    ret += " " + num::str(this->selector);
    ret += " " + num::str(this->matching_type);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::TLSA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::TLSA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void chen::dns::TLSA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void chen::dns::TLSA::unpack(const json::object &object)
{
    map::write(object, "usage", this->usage);
    map::write(object, "selector", this->selector);
    map::write(object, "matching_type", this->matching_type);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// SMIMEA
chen::dns::SMIMEA::SMIMEA() : RR(RRType::SMIMEA)
{
}

std::string chen::dns::SMIMEA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->usage);
    ret += " " + num::str(this->selector);
    ret += " " + num::str(this->matching_type);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::SMIMEA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SMIMEA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void chen::dns::SMIMEA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void chen::dns::SMIMEA::unpack(const json::object &object)
{
    map::write(object, "usage", this->usage);
    map::write(object, "selector", this->selector);
    map::write(object, "matching_type", this->matching_type);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// HIP
chen::dns::HIP::HIP() : RR(RRType::HIP)
{
}

std::string chen::dns::HIP::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::HIP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::HIP::pack(dns::encoder &encoder) const
{
    encoder.pack(this->hit_length);
    encoder.pack(this->pk_algorithm);
    encoder.pack(this->pk_length);
    encoder.pack(this->hit, codec::StringType::Plain, false);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
    encoder.pack(this->rendezvous_servers, codec::StringType::Plain, false);
}

void chen::dns::HIP::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->hit_length);
    decoder.unpack(this->pk_algorithm);
    decoder.unpack(this->pk_length);
    decoder.unpack(this->hit, codec::StringType::Plain);
    decoder.unpack(this->publickey, codec::StringType::Plain);
    decoder.unpack(this->rendezvous_servers, codec::StringType::Plain);
}

void chen::dns::HIP::unpack(const json::object &object)
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
chen::dns::NINFO::NINFO() : RR(RRType::NINFO)
{
}

std::string chen::dns::NINFO::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->zs_data;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NINFO::pack(dns::encoder &encoder) const
{
    encoder.pack(this->zs_data, codec::StringType::Plain, false);
}

void chen::dns::NINFO::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->zs_data, codec::StringType::Plain);
}

void chen::dns::NINFO::unpack(const json::object &object)
{
    map::write(object, "zs_data", this->zs_data);
}


// -----------------------------------------------------------------------------
// RKEY
chen::dns::RKEY::RKEY() : RR(RRType::RKEY)
{
}

std::string chen::dns::RKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::RKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::RKEY::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::RKEY::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::RKEY::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// TALINK
chen::dns::TALINK::TALINK() : RR(RRType::TALINK)
{
}

std::string chen::dns::TALINK::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + this->previous_name;
    ret += " " + this->next_name;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::TALINK::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::TALINK::pack(dns::encoder &encoder) const
{
    encoder.pack(this->previous_name, codec::StringType::Domain, false);
    encoder.pack(this->next_name, codec::StringType::Domain, false);
}

void chen::dns::TALINK::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->previous_name, codec::StringType::Domain);
    decoder.unpack(this->next_name, codec::StringType::Domain);
}

void chen::dns::TALINK::unpack(const json::object &object)
{
    map::write(object, "previous_name", this->previous_name);
    map::write(object, "next_name", this->next_name);
}


// -----------------------------------------------------------------------------
// CDS
chen::dns::CDS::CDS() : RR(RRType::CDS)
{
}

std::string chen::dns::CDS::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::CDS::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::CDS::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::CDS::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::CDS::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// CDNSKEY
chen::dns::CDNSKEY::CDNSKEY() : RR(RRType::CDNSKEY)
{
}

std::string chen::dns::CDNSKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::CDNSKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::CDNSKEY::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::CDNSKEY::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::CDNSKEY::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
chen::dns::OPENPGPKEY::OPENPGPKEY() : RR(RRType::OPENPGPKEY)
{
}

std::string chen::dns::OPENPGPKEY::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + this->publickey;
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::OPENPGPKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::OPENPGPKEY::pack(dns::encoder &encoder) const
{
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::OPENPGPKEY::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::OPENPGPKEY::unpack(const json::object &object)
{
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// CSYNC
chen::dns::CSYNC::CSYNC() : RR(RRType::CSYNC)
{
}

std::string chen::dns::CSYNC::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->serial);
    ret += " " + num::str(this->flags);
    ret += " " + RR::escape(this->type_bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::CSYNC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::CSYNC::pack(dns::encoder &encoder) const
{
    encoder.pack(this->serial);
    encoder.pack(this->flags);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void chen::dns::CSYNC::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->serial);
    decoder.unpack(this->flags);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(std::distance(tmp, decoder.cur())));
}

void chen::dns::CSYNC::unpack(const json::object &object)
{
    map::write(object, "serial", this->serial);
    map::write(object, "flags", this->flags);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// SPF
chen::dns::SPF::SPF() : RR(RRType::SPF)
{
}

std::string chen::dns::SPF::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->txt);
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::SPF::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::SPF::pack(dns::encoder &encoder) const
{
    encoder.pack(this->txt, codec::StringType::Plain, false);
}

void chen::dns::SPF::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->txt, codec::StringType::Plain);
}

void chen::dns::SPF::unpack(const json::object &object)
{
    map::write(object, "txt", this->txt);
}


// -----------------------------------------------------------------------------
// UINFO
chen::dns::UINFO::UINFO() : Raw(RRType::UINFO)
{
}

std::shared_ptr<chen::dns::RR> chen::dns::UINFO::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// UID
chen::dns::UID::UID() : Raw(RRType::UID)
{
}

std::shared_ptr<chen::dns::RR> chen::dns::UID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// GID
chen::dns::GID::GID() : Raw(RRType::GID)
{
}

std::shared_ptr<chen::dns::RR> chen::dns::GID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// UNSPEC
chen::dns::UNSPEC::UNSPEC() : Raw(RRType::UNSPEC)
{
}

std::shared_ptr<chen::dns::RR> chen::dns::UNSPEC::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// NID
chen::dns::NID::NID() : RR(RRType::NID)
{
}

std::string chen::dns::NID::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->node_id);

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::NID::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::NID::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->node_id);
}

void chen::dns::NID::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->node_id);
}

void chen::dns::NID::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "node_id", this->node_id);
}


// -----------------------------------------------------------------------------
// L32
chen::dns::L32::L32() : RR(RRType::L32)
{
}

std::string chen::dns::L32::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->locator32);

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::L32::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::L32::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator32);
}

void chen::dns::L32::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->locator32);
}

void chen::dns::L32::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "locator32", this->locator32);
}


// -----------------------------------------------------------------------------
// L64
chen::dns::L64::L64() : RR(RRType::L64)
{
}

std::string chen::dns::L64::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->locator64);

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::L64::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::L64::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator64);
}

void chen::dns::L64::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->locator64);
}

void chen::dns::L64::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "locator64", this->locator64);
}


// -----------------------------------------------------------------------------
// LP
chen::dns::LP::LP() : RR(RRType::LP)
{
}

std::string chen::dns::LP::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->fqdn;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::LP::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::LP::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->fqdn, codec::StringType::Domain, false);
}

void chen::dns::LP::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->fqdn, codec::StringType::Domain);
}

void chen::dns::LP::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "fqdn", this->fqdn);
}


// -----------------------------------------------------------------------------
// EUI48
chen::dns::EUI48::EUI48() : RR(RRType::EUI48)
{
}

std::string chen::dns::EUI48::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + RR::escape(this->address.size());
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::EUI48::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::EUI48::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::EUI48::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::EUI48::unpack(const json::object &object)
{
    std::string address = map::find(object, "address", std::string());
    std::copy(address.begin(), address.end(), this->address.begin());
}


// -----------------------------------------------------------------------------
// EUI64
chen::dns::EUI64::EUI64() : RR(RRType::EUI64)
{
}

std::string chen::dns::EUI64::str(const std::string &sep) const
{
    auto ret = RR::str(sep);
    ret += sep + num::str(this->address);
    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::EUI64::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::EUI64::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::EUI64::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::EUI64::unpack(const json::object &object)
{
    map::write(object, "address", this->address);
}


// -----------------------------------------------------------------------------
// TKEY
chen::dns::TKEY::TKEY() : RR(RRType::TKEY)
{
}

std::string chen::dns::TKEY::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::TKEY::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::TKEY::pack(dns::encoder &encoder) const
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

void chen::dns::TKEY::unpack(dns::decoder &decoder)
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

void chen::dns::TKEY::unpack(const json::object &object)
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
chen::dns::TSIG::TSIG() : RR(RRType::TSIG)
{
}

std::string chen::dns::TSIG::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::RR> chen::dns::TSIG::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::TSIG::pack(dns::encoder &encoder) const
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

void chen::dns::TSIG::unpack(dns::decoder &decoder)
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

void chen::dns::TSIG::unpack(const json::object &object)
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
chen::dns::URI::URI() : RR(RRType::URI)
{
}

std::string chen::dns::URI::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->priority);
    ret += " " + num::str(this->weight);
    ret += " " + this->target;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::URI::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::URI::pack(dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->target, codec::StringType::Plain, false);
}

void chen::dns::URI::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->target, codec::StringType::Plain);
}

void chen::dns::URI::unpack(const json::object &object)
{
    map::write(object, "priority", this->priority);
    map::write(object, "weight", this->weight);
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// CAA
chen::dns::CAA::CAA() : RR(RRType::CAA)
{
}

std::string chen::dns::CAA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + this->tag;
    ret += " " + this->value;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::CAA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::CAA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->tag, codec::StringType::Plain, false);
    encoder.pack(this->value, codec::StringType::Plain, false);
}

void chen::dns::CAA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->tag, codec::StringType::Plain);
    decoder.unpack(this->value, codec::StringType::Plain);
}

void chen::dns::CAA::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "tag", this->tag);
    map::write(object, "value", this->value);
}


// -----------------------------------------------------------------------------
// TA
chen::dns::TA::TA() : RR(RRType::TA)
{
}

std::string chen::dns::TA::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::TA::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::TA::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::TA::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::TA::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// DLV
chen::dns::DLV::DLV() : RR(RRType::DLV)
{
}

std::string chen::dns::DLV::str(const std::string &sep) const
{
    auto ret = RR::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::RR> chen::dns::DLV::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::DLV::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::DLV::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::DLV::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}