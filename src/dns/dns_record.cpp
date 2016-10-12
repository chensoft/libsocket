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
// rr
chen::dns::rr::rr(RRType type) : rrtype(type)
{
}

// encode & decode
void chen::dns::rr::encode(dns::encoder &encoder) const
{
    // according to rfc1035, section 4.1.4
    // pointers can only be used for occurrence of a domain name where the format is not class specific
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

void chen::dns::rr::decode(dns::decoder &decoder)
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

void chen::dns::rr::decode(const json::object &object)
{
    // unpack base fields, ignore rdlength
    map::write(object, "name", this->name);
    map::write(object, "rrtype", reinterpret_cast<std::uint16_t&>(this->rrtype));
    map::write(object, "rrclass", reinterpret_cast<std::uint16_t&>(this->rrclass));
    map::write(object, "ttl", this->ttl);

    // unpack subclass
    this->unpack(object);
}

std::shared_ptr<chen::dns::rr> chen::dns::rr::create(dns::decoder &decoder)
{
    dns::decoder detect(decoder);

    // detect type
    std::string unused;
    detect.unpack(unused, codec::StringType::Domain);

    RRType rrtype = TypeNone;
    detect.unpack(rrtype);

    // build record
    std::shared_ptr<rr> record = table::build(rrtype);
    if (!record)
        return nullptr;

    // decode it
    record->decode(decoder);

    return record;
}

// str
std::string chen::dns::rr::str(const std::string &sep) const
{
    std::string ret;

    ret += this->name + sep;
    ret += num::str(this->ttl) + sep;
    ret += table::classToText(this->rrclass) + sep;
    ret += table::typeToText(this->rrtype);

    return ret;
}

// helper
std::size_t chen::dns::rr::remain(std::size_t used) const
{
    if (this->rdlength < used)
        throw std::runtime_error("dns: codec rdata is overflow");

    return static_cast<std::size_t>(this->rdlength - used);
}

std::string chen::dns::rr::escape(const std::vector<std::uint8_t> &data)
{
    std::string ret;

    for (auto ch : data)
        ret += str::format("%02X", ch);

    return ret;
}

std::string chen::dns::rr::escape(const std::string &text)
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

std::string chen::dns::rr::escape(std::size_t bits)
{
    return "<<" + num::str(bits) + "bits>>";
}


// -----------------------------------------------------------------------------
// raw
chen::dns::raw::raw() : raw(TypeNone)
{
}

chen::dns::raw::raw(RRType type) : rr(type)
{
}

std::string chen::dns::raw::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + rr::escape(this->rdata.size());
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::raw::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::raw::pack(dns::encoder &encoder) const
{
    encoder.pack(this->rdata, this->rdata.size());
}

void chen::dns::raw::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->rdata, this->rdlength);
}

void chen::dns::raw::unpack(const json::object &object)
{
    this->rdata.clear();

    std::string rdata = map::find(object, "rdata");
    std::copy(rdata.begin(), rdata.end(), this->rdata.begin());
}


// -----------------------------------------------------------------------------
// A
chen::dns::rr_a::rr_a() : rr(TypeA)
{
}

std::string chen::dns::rr_a::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + ip::version4::toString(this->address);
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_a::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_a::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::rr_a::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::rr_a::unpack(const json::object &object)
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
chen::dns::rr_ns::rr_ns() : rr(TypeNS)
{
}

std::string chen::dns::rr_ns::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->nsdname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_ns::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_ns::pack(dns::encoder &encoder) const
{
    encoder.pack(this->nsdname, codec::StringType::Domain, false);
}

void chen::dns::rr_ns::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->nsdname, codec::StringType::Domain);
}

void chen::dns::rr_ns::unpack(const json::object &object)
{
    map::write(object, "nsdname", this->nsdname);
}


// -----------------------------------------------------------------------------
// MD
chen::dns::rr_md::rr_md() : rr(TypeMD)
{
}

std::string chen::dns::rr_md::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->madname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_md::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_md::pack(dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void chen::dns::rr_md::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void chen::dns::rr_md::unpack(const json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// MF
chen::dns::rr_mf::rr_mf() : rr(TypeMF)
{
}

std::string chen::dns::rr_mf::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += this->madname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_mf::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_mf::pack(dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void chen::dns::rr_mf::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void chen::dns::rr_mf::unpack(const json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// CNAME
chen::dns::rr_cname::rr_cname() : rr(TypeCNAME)
{
}

std::string chen::dns::rr_cname::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->cname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_cname::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_cname::pack(dns::encoder &encoder) const
{
    encoder.pack(this->cname, codec::StringType::Domain, false);
}

void chen::dns::rr_cname::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->cname, codec::StringType::Domain);
}

void chen::dns::rr_cname::unpack(const json::object &object)
{
    map::write(object, "cname", this->cname);
}


// -----------------------------------------------------------------------------
// SOA
chen::dns::rr_soa::rr_soa() : rr(TypeSOA)
{
}

std::string chen::dns::rr_soa::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + this->mname;
    ret += " " + this->rname;  // use a space to seprate different fields
    ret += " " + num::str(this->serial);
    ret += " " + num::str(this->refresh);
    ret += " " + num::str(this->retry);
    ret += " " + num::str(this->expire);
    ret += " " + num::str(this->minimum);

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_soa::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_soa::pack(dns::encoder &encoder) const
{
    encoder.pack(this->mname, codec::StringType::Domain, false);
    encoder.pack(this->rname, codec::StringType::Domain, false);
    encoder.pack(this->serial);
    encoder.pack(this->refresh);
    encoder.pack(this->retry);
    encoder.pack(this->expire);
    encoder.pack(this->minimum);
}

void chen::dns::rr_soa::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->mname, codec::StringType::Domain);
    decoder.unpack(this->rname, codec::StringType::Domain);
    decoder.unpack(this->serial);
    decoder.unpack(this->refresh);
    decoder.unpack(this->retry);
    decoder.unpack(this->expire);
    decoder.unpack(this->minimum);
}

void chen::dns::rr_soa::unpack(const json::object &object)
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
chen::dns::rr_mb::rr_mb() : rr(TypeMB)
{
}

std::string chen::dns::rr_mb::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->madname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_mb::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_mb::pack(dns::encoder &encoder) const
{
    encoder.pack(this->madname, codec::StringType::Domain, false);
}

void chen::dns::rr_mb::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->madname, codec::StringType::Domain);
}

void chen::dns::rr_mb::unpack(const json::object &object)
{
    map::write(object, "madname", this->madname);
}


// -----------------------------------------------------------------------------
// MG
chen::dns::rr_mg::rr_mg() : rr(TypeMG)
{
}

std::string chen::dns::rr_mg::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->mgmname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_mg::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_mg::pack(dns::encoder &encoder) const
{
    encoder.pack(this->mgmname, codec::StringType::Domain, false);
}

void chen::dns::rr_mg::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->mgmname, codec::StringType::Domain);
}

void chen::dns::rr_mg::unpack(const json::object &object)
{
    map::write(object, "mgmname", this->mgmname);
}


// -----------------------------------------------------------------------------
// MR
chen::dns::rr_mr::rr_mr() : rr(TypeMR)
{
}

std::string chen::dns::rr_mr::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->newname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_mr::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_mr::pack(dns::encoder &encoder) const
{
    encoder.pack(this->newname, codec::StringType::Domain, false);
}

void chen::dns::rr_mr::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->newname, codec::StringType::Domain);
}

void chen::dns::rr_mr::unpack(const json::object &object)
{
    map::write(object, "newname", this->newname);
}


// -----------------------------------------------------------------------------
// Null
chen::dns::rr_null::rr_null() : raw(TypeNULL)
{
}

std::string chen::dns::rr_null::str(const std::string &sep) const
{
    auto ret = raw::str(sep);
    ret += sep + rr::escape(this->anything.size());
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_null::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_null::pack(dns::encoder &encoder) const
{
    // base
    raw::pack(encoder);

    // self
    encoder.pack(this->anything, this->anything.size());
}

void chen::dns::rr_null::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->anything, this->rdlength);
}

void chen::dns::rr_null::unpack(const json::object &object)
{
    raw::unpack(object);

    this->anything.clear();

    std::string anything = map::find(object, "anything", std::string());
    std::copy(anything.begin(), anything.end(), this->anything.begin());
}


// -----------------------------------------------------------------------------
// WKS
chen::dns::rr_wks::rr_wks() : rr(TypeWKS)
{
}

std::string chen::dns::rr_wks::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->address);
    ret += " " + num::str(this->protocol);
    ret += " " + rr::escape(this->bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_wks::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_wks::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
    encoder.pack(this->protocol);
    encoder.pack(this->bitmap, this->bitmap.size());
}

void chen::dns::rr_wks::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->address);
    decoder.unpack(this->protocol);

    this->bitmap.clear();
    decoder.unpack(this->bitmap, this->remain(static_cast<std::size_t>(std::distance(tmp, decoder.cur()))));
}

void chen::dns::rr_wks::unpack(const json::object &object)
{
    map::write(object, "address", this->address);
    map::write(object, "protocol", this->protocol);

    this->bitmap.clear();

    std::string bitmap = map::find(object, "bitmap", std::string());
    std::copy(bitmap.begin(), bitmap.end(), this->bitmap.begin());
}


// -----------------------------------------------------------------------------
// PTR
chen::dns::rr_ptr::rr_ptr() : rr(TypePTR)
{
}

std::string chen::dns::rr_ptr::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->ptrdname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_ptr::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_ptr::pack(dns::encoder &encoder) const
{
    encoder.pack(this->ptrdname, codec::StringType::Domain, false);
}

void chen::dns::rr_ptr::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->ptrdname, codec::StringType::Domain);
}

void chen::dns::rr_ptr::unpack(const json::object &object)
{
    map::write(object, "ptrdname", this->ptrdname);
}


// -----------------------------------------------------------------------------
// HINFO
chen::dns::rr_hinfo::rr_hinfo() : rr(TypeHINFO)
{
}

std::string chen::dns::rr_hinfo::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->cpu;
    ret += " " + this->os;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_hinfo::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_hinfo::pack(dns::encoder &encoder) const
{
    encoder.pack(this->cpu, codec::StringType::Plain, false);
    encoder.pack(this->os, codec::StringType::Plain, false);
}

void chen::dns::rr_hinfo::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->cpu, codec::StringType::Plain);
    decoder.unpack(this->os, codec::StringType::Plain);
}

void chen::dns::rr_hinfo::unpack(const json::object &object)
{
    map::write(object, "cpu", this->cpu);
    map::write(object, "os", this->os);
}


// -----------------------------------------------------------------------------
// MINFO
chen::dns::rr_minfo::rr_minfo() : rr(TypeMINFO)
{
}

std::string chen::dns::rr_minfo::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->rmailbx;
    ret += " " + this->emailbx;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_minfo::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_minfo::pack(dns::encoder &encoder) const
{
    encoder.pack(this->rmailbx, codec::StringType::Domain, false);
    encoder.pack(this->emailbx, codec::StringType::Domain, false);
}

void chen::dns::rr_minfo::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->rmailbx, codec::StringType::Domain);
    decoder.unpack(this->emailbx, codec::StringType::Domain);
}

void chen::dns::rr_minfo::unpack(const json::object &object)
{
    map::write(object, "rmailbx", this->rmailbx);
    map::write(object, "emailbx", this->emailbx);
}


// -----------------------------------------------------------------------------
// MX
chen::dns::rr_mx::rr_mx() : rr(TypeMX)
{
}

std::string chen::dns::rr_mx::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + num::str(this->preference);
    ret += " " + this->exchange;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_mx::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_mx::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchange, codec::StringType::Domain, false);
}

void chen::dns::rr_mx::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->exchange, codec::StringType::Domain);
}

void chen::dns::rr_mx::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "exchange", this->exchange);
}


// -----------------------------------------------------------------------------
// TXT
chen::dns::rr_txt::rr_txt() : rr(TypeTXT)
{
}

std::string chen::dns::rr_txt::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + rr::escape(this->txt_data);
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_txt::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_txt::pack(dns::encoder &encoder) const
{
    encoder.pack(this->txt_data, codec::StringType::Plain, false);
}

void chen::dns::rr_txt::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->txt_data, codec::StringType::Plain);
}

void chen::dns::rr_txt::unpack(const json::object &object)
{
    map::write(object, "txt_data", this->txt_data);
}


// -----------------------------------------------------------------------------
// RP
chen::dns::rr_rp::rr_rp() : rr(TypeRP)
{
}

std::string chen::dns::rr_rp::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->mbox_dname;
    ret += " " + this->txt_dname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_rp::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_rp::pack(dns::encoder &encoder) const
{
    encoder.pack(this->mbox_dname, codec::StringType::Domain, false);
    encoder.pack(this->txt_dname, codec::StringType::Domain, false);
}

void chen::dns::rr_rp::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->mbox_dname, codec::StringType::Domain);
    decoder.unpack(this->txt_dname, codec::StringType::Domain);
}

void chen::dns::rr_rp::unpack(const json::object &object)
{
    map::write(object, "mbox_dname", this->mbox_dname);
    map::write(object, "txt_dname", this->txt_dname);
}


// -----------------------------------------------------------------------------
// AFSDB
chen::dns::rr_afsdb::rr_afsdb() : rr(TypeAFSDB)
{
}

std::string chen::dns::rr_afsdb::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + num::str(this->subtype);
    ret += " " + this->hostname;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_afsdb::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_afsdb::pack(dns::encoder &encoder) const
{
    encoder.pack(this->subtype);
    encoder.pack(this->hostname, codec::StringType::Domain, false);
}

void chen::dns::rr_afsdb::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->subtype);
    decoder.unpack(this->hostname, codec::StringType::Domain);
}

void chen::dns::rr_afsdb::unpack(const json::object &object)
{
    map::write(object, "subtype", this->subtype);
    map::write(object, "hostname", this->hostname);
}


// -----------------------------------------------------------------------------
// X25
chen::dns::rr_x25::rr_x25() : rr(TypeX25)
{
}

std::string chen::dns::rr_x25::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->psdn_address;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_x25::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_x25::pack(dns::encoder &encoder) const
{
    encoder.pack(this->psdn_address, codec::StringType::Plain, false);
}

void chen::dns::rr_x25::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->psdn_address, codec::StringType::Plain);
}

void chen::dns::rr_x25::unpack(const json::object &object)
{
    map::write(object, "psdn_address", this->psdn_address);
}


// -----------------------------------------------------------------------------
// ISDN
chen::dns::rr_isdn::rr_isdn() : rr(TypeISDN)
{
}

std::string chen::dns::rr_isdn::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->isdn_address;
    ret += " " + this->sa;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_isdn::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_isdn::pack(dns::encoder &encoder) const
{
    encoder.pack(this->isdn_address, codec::StringType::Plain, false);
    encoder.pack(this->sa, codec::StringType::Plain, false);
}

void chen::dns::rr_isdn::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->isdn_address, codec::StringType::Plain);
    decoder.unpack(this->sa, codec::StringType::Plain);
}

void chen::dns::rr_isdn::unpack(const json::object &object)
{
    map::write(object, "isdn_address", this->isdn_address);
    map::write(object, "sa", this->sa);
}


// -----------------------------------------------------------------------------
// RT
chen::dns::rr_rt::rr_rt() : rr(TypeRT)
{
}

std::string chen::dns::rr_rt::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + num::str(this->preference);
    ret += " " + this->intermediate_host;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_rt::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_rt::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->intermediate_host, codec::StringType::Domain, false);
}

void chen::dns::rr_rt::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->intermediate_host, codec::StringType::Domain);
}

void chen::dns::rr_rt::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "intermediate_host", this->intermediate_host);
}


// -----------------------------------------------------------------------------
// NSAP
chen::dns::rr_nsap::rr_nsap() : rr(TypeNSAP)
{
}

std::string chen::dns::rr_nsap::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->nsap;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nsap::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nsap::pack(dns::encoder &encoder) const
{
    encoder.pack(this->nsap, codec::StringType::Plain, false);
}

void chen::dns::rr_nsap::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->nsap, codec::StringType::Plain);
}

void chen::dns::rr_nsap::unpack(const json::object &object)
{
    map::write(object, "nsap", this->nsap);
}


// -----------------------------------------------------------------------------
// NSAPPTR
chen::dns::rr_nsapptr::rr_nsapptr() : rr(TypeNSAPPTR)
{
}

std::string chen::dns::rr_nsapptr::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->owner;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nsapptr::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nsapptr::pack(dns::encoder &encoder) const
{
    encoder.pack(this->owner, codec::StringType::Domain, false);
}

void chen::dns::rr_nsapptr::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->owner, codec::StringType::Domain);
}

void chen::dns::rr_nsapptr::unpack(const json::object &object)
{
    map::write(object, "owner", this->owner);
}


// -----------------------------------------------------------------------------
// SIG
chen::dns::rr_sig::rr_sig() : rr(TypeSIG)
{
}

std::string chen::dns::rr_sig::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

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

std::shared_ptr<chen::dns::rr> chen::dns::rr_sig::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_sig::pack(dns::encoder &encoder) const
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

void chen::dns::rr_sig::unpack(dns::decoder &decoder)
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

void chen::dns::rr_sig::unpack(const json::object &object)
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
chen::dns::rr_key::rr_key() : rr(TypeKEY)
{
}

std::string chen::dns::rr_key::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_key::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_key::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::rr_key::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::rr_key::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// PX
chen::dns::rr_px::rr_px() : rr(TypePX)
{
}

std::string chen::dns::rr_px::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->map822;
    ret += " " + this->mapx400;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_px::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_px::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->map822, codec::StringType::Domain, false);
    encoder.pack(this->mapx400, codec::StringType::Domain, false);
}

void chen::dns::rr_px::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->map822, codec::StringType::Domain);
    decoder.unpack(this->mapx400, codec::StringType::Domain);
}

void chen::dns::rr_px::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "map822", this->map822);
    map::write(object, "mapx400", this->mapx400);
}


// -----------------------------------------------------------------------------
// GPOS
chen::dns::rr_gpos::rr_gpos() : rr(TypeGPOS)
{
}

std::string chen::dns::rr_gpos::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + this->longitude;
    ret += " " + this->latitude;
    ret += " " + this->altitude;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_gpos::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_gpos::pack(dns::encoder &encoder) const
{
    encoder.pack(this->longitude, codec::StringType::Plain, false);
    encoder.pack(this->latitude, codec::StringType::Plain, false);
    encoder.pack(this->altitude, codec::StringType::Plain, false);
}

void chen::dns::rr_gpos::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->longitude, codec::StringType::Plain);
    decoder.unpack(this->latitude, codec::StringType::Plain);
    decoder.unpack(this->altitude, codec::StringType::Plain);
}

void chen::dns::rr_gpos::unpack(const json::object &object)
{
    map::write(object, "longitude", this->longitude);
    map::write(object, "latitude", this->latitude);
    map::write(object, "altitude", this->altitude);
}


// -----------------------------------------------------------------------------
// AAAA
chen::dns::rr_aaaa::rr_aaaa() : rr(TypeAAAA)
{
}

std::string chen::dns::rr_aaaa::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + ip::version6::toString(this->address.data());
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_aaaa::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_aaaa::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::rr_aaaa::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::rr_aaaa::unpack(const json::object &object)
{
    auto address = map::find(object, "address");
    this->address = ip::version6::toBytes(address);
}


// -----------------------------------------------------------------------------
// LOC
chen::dns::rr_loc::rr_loc() : rr(TypeLOC)
{
}

std::string chen::dns::rr_loc::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->version);
    ret += " " + num::str(this->size);
    ret += " " + num::str(this->horiz_pre);
    ret += " " + num::str(this->vert_pre);
    ret += " " + num::str(this->longitude);
    ret += " " + num::str(this->latitude);
    ret += " " + num::str(this->altitude);

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_loc::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_loc::pack(dns::encoder &encoder) const
{
    encoder.pack(this->version);
    encoder.pack(this->size);
    encoder.pack(this->horiz_pre);
    encoder.pack(this->vert_pre);
    encoder.pack(this->longitude);
    encoder.pack(this->latitude);
    encoder.pack(this->altitude);
}

void chen::dns::rr_loc::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->version);
    decoder.unpack(this->size);
    decoder.unpack(this->horiz_pre);
    decoder.unpack(this->vert_pre);
    decoder.unpack(this->longitude);
    decoder.unpack(this->latitude);
    decoder.unpack(this->altitude);
}

void chen::dns::rr_loc::unpack(const json::object &object)
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
chen::dns::rr_nxt::rr_nxt() : rr(TypeNXT)
{
}

std::string chen::dns::rr_nxt::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->next_domain;
    ret += " " + rr::escape(this->type_bitmap.size());
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nxt::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nxt::pack(dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, codec::StringType::Domain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void chen::dns::rr_nxt::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->next_domain, codec::StringType::Domain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(static_cast<std::size_t>(std::distance(tmp, decoder.cur()))));
}

void chen::dns::rr_nxt::unpack(const json::object &object)
{
    map::write(object, "next_domain", this->next_domain);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// EID
chen::dns::rr_eid::rr_eid() : rr(TypeEID)
{
}

std::string chen::dns::rr_eid::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->endpoint;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_eid::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_eid::pack(dns::encoder &encoder) const
{
    encoder.pack(this->endpoint, codec::StringType::Plain, false);
}

void chen::dns::rr_eid::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->endpoint, codec::StringType::Plain);
}

void chen::dns::rr_eid::unpack(const json::object &object)
{
    map::write(object, "endpoint", this->endpoint);
}


// -----------------------------------------------------------------------------
// NIMLOC
chen::dns::rr_nimloc::rr_nimloc() : rr(TypeNIMLOC)
{
}

std::string chen::dns::rr_nimloc::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->locator;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nimloc::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nimloc::pack(dns::encoder &encoder) const
{
    encoder.pack(this->locator, codec::StringType::Plain, false);
}

void chen::dns::rr_nimloc::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->locator, codec::StringType::Plain);
}

void chen::dns::rr_nimloc::unpack(const json::object &object)
{
    map::write(object, "locator", this->locator);
}


// -----------------------------------------------------------------------------
// SRV
chen::dns::rr_srv::rr_srv() : rr(TypeSRV)
{
}

std::string chen::dns::rr_srv::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->priority);
    ret += " " + num::str(this->weight);
    ret += " " + num::str(this->port);
    ret += " " + this->target;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_srv::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_srv::pack(dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->port);
    encoder.pack(this->target, codec::StringType::Domain, false);
}

void chen::dns::rr_srv::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->port);
    decoder.unpack(this->target, codec::StringType::Domain);
}

void chen::dns::rr_srv::unpack(const json::object &object)
{
    map::write(object, "priority", this->priority);
    map::write(object, "weight", this->weight);
    map::write(object, "port", this->port);
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// ATMA
chen::dns::rr_atma::rr_atma() : rr(TypeATMA)
{
}

std::string chen::dns::rr_atma::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->format);
    ret += " " + this->address;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_atma::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_atma::pack(dns::encoder &encoder) const
{
    encoder.pack(this->format);
    encoder.pack(this->address, codec::StringType::Plain, false);
}

void chen::dns::rr_atma::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->format);
    decoder.unpack(this->address, codec::StringType::Plain);
}

void chen::dns::rr_atma::unpack(const json::object &object)
{
    map::write(object, "format", this->format);
    map::write(object, "address", this->address);
}


// -----------------------------------------------------------------------------
// NAPTR
chen::dns::rr_naptr::rr_naptr() : rr(TypeNAPTR)
{
}

std::string chen::dns::rr_naptr::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->order);
    ret += " " + num::str(this->preference);
    ret += " " + this->flags;
    ret += " " + this->services;
    ret += " " + this->regexp;
    ret += " " + this->replacement;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_naptr::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_naptr::pack(dns::encoder &encoder) const
{
    encoder.pack(this->order);
    encoder.pack(this->preference);
    encoder.pack(this->flags, codec::StringType::Plain, false);
    encoder.pack(this->services, codec::StringType::Plain, false);
    encoder.pack(this->regexp, codec::StringType::Plain, false);
    encoder.pack(this->replacement, codec::StringType::Domain, false);
}

void chen::dns::rr_naptr::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->order);
    decoder.unpack(this->preference);
    decoder.unpack(this->flags, codec::StringType::Plain);
    decoder.unpack(this->services, codec::StringType::Plain);
    decoder.unpack(this->regexp, codec::StringType::Plain);
    decoder.unpack(this->replacement, codec::StringType::Domain);
}

void chen::dns::rr_naptr::unpack(const json::object &object)
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
chen::dns::rr_kx::rr_kx() : rr(TypeKX)
{
}

std::string chen::dns::rr_kx::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->exchanger;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_kx::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_kx::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->exchanger, codec::StringType::Domain, false);
}

void chen::dns::rr_kx::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->exchanger, codec::StringType::Domain);
}

void chen::dns::rr_kx::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "exchanger", this->exchanger);
}


// -----------------------------------------------------------------------------
// CERT
chen::dns::rr_cert::rr_cert() : rr(TypeCERT)
{
}

std::string chen::dns::rr_cert::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->type);
    ret += " " + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_cert::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_cert::pack(dns::encoder &encoder) const
{
    encoder.pack(this->type);
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void chen::dns::rr_cert::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->type);
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void chen::dns::rr_cert::unpack(const json::object &object)
{
    map::write(object, "type", this->type);
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// A6
chen::dns::rr_a6::rr_a6() : rr(TypeA6)
{
}

std::string chen::dns::rr_a6::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->prefix);
    ret += " " + rr::escape(this->suffix.size());
    ret += " " + this->prefix_name;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_a6::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_a6::pack(dns::encoder &encoder) const
{
    encoder.pack(this->prefix);
    encoder.pack(this->suffix, this->suffix.size());
    encoder.pack(this->prefix_name, codec::StringType::Domain, false);
}

void chen::dns::rr_a6::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->prefix);

    this->suffix.clear();
    decoder.unpack(this->suffix, static_cast<std::size_t>(128 - this->prefix));

    decoder.unpack(this->prefix_name, codec::StringType::Domain);
}

void chen::dns::rr_a6::unpack(const json::object &object)
{
    map::write(object, "prefix", this->prefix);

    this->suffix.clear();

    std::string suffix = map::find(object, "suffix", std::string());
    std::copy(suffix.begin(), suffix.end(), this->suffix.begin());

    map::write(object, "prefix_name", this->prefix_name);
}


// -----------------------------------------------------------------------------
// DNAME
chen::dns::rr_dname::rr_dname() : rr(TypeDNAME)
{
}

std::string chen::dns::rr_dname::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->target;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_dname::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_dname::pack(dns::encoder &encoder) const
{
    encoder.pack(this->target, codec::StringType::Domain, false);
}

void chen::dns::rr_dname::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->target, codec::StringType::Domain);
}

void chen::dns::rr_dname::unpack(const json::object &object)
{
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// SINK
chen::dns::rr_sink::rr_sink() : rr(TypeSINK)
{
}

std::string chen::dns::rr_sink::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->coding);
    ret += " " + num::str(this->subcoding);
    ret += " " + rr::escape(this->sdata.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_sink::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_sink::pack(dns::encoder &encoder) const
{
    encoder.pack(this->coding);
    encoder.pack(this->subcoding);
    encoder.pack(this->sdata, this->sdata.size());
}

void chen::dns::rr_sink::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->coding);
    decoder.unpack(this->subcoding);

    this->sdata.clear();
    decoder.unpack(this->sdata, this->remain(static_cast<std::size_t>(std::distance(tmp, decoder.cur()))));
}

void chen::dns::rr_sink::unpack(const json::object &object)
{
    map::write(object, "coding", this->coding);
    map::write(object, "subcoding", this->subcoding);

    this->sdata.clear();

    std::string sdata = map::find(object, "sdata", std::string());
    std::copy(sdata.begin(), sdata.end(), this->sdata.begin());
}


// -----------------------------------------------------------------------------
// OPT
chen::dns::rr_opt::rr_opt() : rr(TypeOPT)
{
    // opt name is always '.'
    this->name = ".";
}

// property
std::uint16_t chen::dns::rr_opt::payload() const
{
    return static_cast<std::uint16_t>(this->rrclass);
}

std::uint16_t chen::dns::rr_opt::rcode(RCode code) const
{
    std::uint16_t ret = this->rcode();
    return ret << 4 | static_cast<std::uint8_t>(code);
}

std::uint8_t chen::dns::rr_opt::rcode() const
{
    return static_cast<std::uint8_t>(this->ttl >> 24 & 0xFF);
}

std::uint8_t chen::dns::rr_opt::version() const
{
    return static_cast<std::uint8_t>(this->ttl >> 16 & 0xFF);
}

std::uint16_t chen::dns::rr_opt::flag() const
{
    return static_cast<std::uint16_t>(this->ttl & 0xFFFF);
}

bool chen::dns::rr_opt::dnssec() const
{
    return (this->flag() & 0x8000) == 0x8000;
}

std::uint16_t chen::dns::rr_opt::z() const
{
    return static_cast<std::uint16_t>(this->flag() & 0x7FFF);
}

void chen::dns::rr_opt::payload(std::uint16_t value)
{
    this->rrclass = static_cast<RRClass>(value);
}

void chen::dns::rr_opt::rcode(std::uint16_t value)
{
    this->rcode(static_cast<std::uint8_t>(value >> 4 & 0xFF));
}

void chen::dns::rr_opt::rcode(std::uint8_t value)
{
    this->ttl = (this->ttl & 0x00FFFFFF) | (static_cast<std::uint32_t>(value) << 24);
}

void chen::dns::rr_opt::version(std::uint8_t value)
{
    this->ttl = (this->ttl & 0xFF00FFFF) | (static_cast<std::uint32_t>(value) << 16);
}

void chen::dns::rr_opt::flag(std::uint16_t value)
{
    this->ttl = (this->ttl & 0xFFFF0000) | value;
}

void chen::dns::rr_opt::dnssec(bool value)
{
    this->ttl &= 0xFFFF7FFF;

    if (value)
        this->ttl |= 0x8000;
}

void chen::dns::rr_opt::z(std::uint16_t value)
{
    this->ttl = (this->ttl & 0x8000) | value;
}

// override
std::string chen::dns::rr_opt::str(const std::string &sep) const
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

std::shared_ptr<chen::dns::rr> chen::dns::rr_opt::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_opt::pack(dns::encoder &encoder) const
{
    for (auto &option : this->options)
        option->encode(encoder);
}

void chen::dns::rr_opt::unpack(dns::decoder &decoder)
{
    this->options.clear();

    while (decoder.cur() != decoder.end())
        this->options.emplace_back(edns0::opt::create(decoder));
}

void chen::dns::rr_opt::unpack(const json::object &object)
{
    throw std::runtime_error("dns: unpack opt record by json is invalid");
}


// -----------------------------------------------------------------------------
// DS
chen::dns::rr_ds::rr_ds() : rr(TypeDS)
{
}

std::string chen::dns::rr_ds::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_ds::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_ds::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::rr_ds::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::rr_ds::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// SSHFP
chen::dns::rr_sshfp::rr_sshfp() : rr(TypeSSHFP)
{
}

std::string chen::dns::rr_sshfp::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->algorithm);
    ret += " " + num::str(this->fptype);
    ret += " " + this->fingerprint;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_sshfp::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_sshfp::pack(dns::encoder &encoder) const
{
    encoder.pack(this->algorithm);
    encoder.pack(this->fptype);
    encoder.pack(this->fingerprint, codec::StringType::Plain, false);
}

void chen::dns::rr_sshfp::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->algorithm);
    decoder.unpack(this->fptype);
    decoder.unpack(this->fingerprint, codec::StringType::Plain);
}

void chen::dns::rr_sshfp::unpack(const json::object &object)
{
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "fptype", this->fptype);
    map::write(object, "fingerprint", this->fingerprint);
}


// -----------------------------------------------------------------------------
// IPSECKEY
chen::dns::rr_ipseckey::rr_ipseckey() : rr(TypeIPSECKEY)
{
}

std::string chen::dns::rr_ipseckey::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->precedence);
    ret += " " + num::str(this->gateway_type);
    ret += " " + num::str(this->algorithm);
    ret += " " + rr::escape(this->gateway.size());
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_ipseckey::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_ipseckey::pack(dns::encoder &encoder) const
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

void chen::dns::rr_ipseckey::unpack(dns::decoder &decoder)
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

void chen::dns::rr_ipseckey::unpack(const json::object &object)
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
chen::dns::rr_rrsig::rr_rrsig() : rr(TypeRRSIG)
{
}

std::string chen::dns::rr_rrsig::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

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

std::shared_ptr<chen::dns::rr> chen::dns::rr_rrsig::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_rrsig::pack(dns::encoder &encoder) const
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

void chen::dns::rr_rrsig::unpack(dns::decoder &decoder)
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

void chen::dns::rr_rrsig::unpack(const json::object &object)
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
chen::dns::rr_nsec::rr_nsec() : rr(TypeNSEC)
{
}

std::string chen::dns::rr_nsec::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->next_domain;
    ret += " " + rr::escape(this->type_bitmap.size());
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nsec::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nsec::pack(dns::encoder &encoder) const
{
    encoder.pack(this->next_domain, codec::StringType::Domain, false);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void chen::dns::rr_nsec::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->next_domain, codec::StringType::Domain);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(static_cast<std::size_t>(std::distance(tmp, decoder.cur()))));
}

void chen::dns::rr_nsec::unpack(const json::object &object)
{
    map::write(object, "next_domain", this->next_domain);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// DNSKEY
chen::dns::rr_dnskey::rr_dnskey() : rr(TypeDNSKEY)
{
}

std::string chen::dns::rr_dnskey::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_dnskey::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_dnskey::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::rr_dnskey::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::rr_dnskey::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// DHCID
chen::dns::rr_dhcid::rr_dhcid() : rr(TypeDHCID)
{
}

std::string chen::dns::rr_dhcid::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->digest;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_dhcid::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_dhcid::pack(dns::encoder &encoder) const
{
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::rr_dhcid::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::rr_dhcid::unpack(const json::object &object)
{
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// NSEC3
chen::dns::rr_nsec3::rr_nsec3() : rr(TypeNSEC3)
{
}

std::string chen::dns::rr_nsec3::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->hash);
    ret += " " + num::str(this->flags);
    ret += " " + num::str(this->iterations);
    ret += " " + num::str(this->salt_length);
    ret += " " + rr::escape(this->salt.size());
    ret += " " + num::str(this->hash_length);
    ret += " " + this->next_owner;
    ret += " " + rr::escape(this->type_bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nsec3::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nsec3::pack(dns::encoder &encoder) const
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

void chen::dns::rr_nsec3::unpack(dns::decoder &decoder)
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
    decoder.unpack(this->type_bitmap, this->remain(static_cast<std::size_t>(std::distance(tmp, decoder.cur()))));
}

void chen::dns::rr_nsec3::unpack(const json::object &object)
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
chen::dns::rr_nsec3param::rr_nsec3param() : rr(TypeNSEC3PARAM)
{
}

std::string chen::dns::rr_nsec3param::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->hash);
    ret += " " + num::str(this->flags);
    ret += " " + num::str(this->iterations);
    ret += " " + num::str(this->salt_length);
    ret += " " + rr::escape(this->salt.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nsec3param::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nsec3param::pack(dns::encoder &encoder) const
{
    encoder.pack(this->hash);
    encoder.pack(this->flags);
    encoder.pack(this->iterations);
    encoder.pack(this->salt_length);
    encoder.pack(this->salt, this->salt_length);
}

void chen::dns::rr_nsec3param::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->hash);
    decoder.unpack(this->flags);
    decoder.unpack(this->iterations);
    decoder.unpack(this->salt_length);
    decoder.unpack(this->salt, this->salt_length);
}

void chen::dns::rr_nsec3param::unpack(const json::object &object)
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
chen::dns::rr_tlsa::rr_tlsa() : rr(TypeTLSA)
{
}

std::string chen::dns::rr_tlsa::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->usage);
    ret += " " + num::str(this->selector);
    ret += " " + num::str(this->matching_type);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_tlsa::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_tlsa::pack(dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void chen::dns::rr_tlsa::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void chen::dns::rr_tlsa::unpack(const json::object &object)
{
    map::write(object, "usage", this->usage);
    map::write(object, "selector", this->selector);
    map::write(object, "matching_type", this->matching_type);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// SMIMEA
chen::dns::rr_smimea::rr_smimea() : rr(TypeSMIMEA)
{
}

std::string chen::dns::rr_smimea::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->usage);
    ret += " " + num::str(this->selector);
    ret += " " + num::str(this->matching_type);
    ret += " " + this->certificate;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_smimea::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_smimea::pack(dns::encoder &encoder) const
{
    encoder.pack(this->usage);
    encoder.pack(this->selector);
    encoder.pack(this->matching_type);
    encoder.pack(this->certificate, codec::StringType::Plain, false);
}

void chen::dns::rr_smimea::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->usage);
    decoder.unpack(this->selector);
    decoder.unpack(this->matching_type);
    decoder.unpack(this->certificate, codec::StringType::Plain);
}

void chen::dns::rr_smimea::unpack(const json::object &object)
{
    map::write(object, "usage", this->usage);
    map::write(object, "selector", this->selector);
    map::write(object, "matching_type", this->matching_type);
    map::write(object, "certificate", this->certificate);
}


// -----------------------------------------------------------------------------
// HIP
chen::dns::rr_hip::rr_hip() : rr(TypeHIP)
{
}

std::string chen::dns::rr_hip::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->hit_length);
    ret += " " + num::str(this->pk_algorithm);
    ret += " " + num::str(this->pk_length);
    ret += " " + this->hit;
    ret += " " + this->publickey;
    ret += " " + this->rendezvous_servers;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_hip::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_hip::pack(dns::encoder &encoder) const
{
    encoder.pack(this->hit_length);
    encoder.pack(this->pk_algorithm);
    encoder.pack(this->pk_length);
    encoder.pack(this->hit, codec::StringType::Plain, false);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
    encoder.pack(this->rendezvous_servers, codec::StringType::Plain, false);
}

void chen::dns::rr_hip::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->hit_length);
    decoder.unpack(this->pk_algorithm);
    decoder.unpack(this->pk_length);
    decoder.unpack(this->hit, codec::StringType::Plain);
    decoder.unpack(this->publickey, codec::StringType::Plain);
    decoder.unpack(this->rendezvous_servers, codec::StringType::Plain);
}

void chen::dns::rr_hip::unpack(const json::object &object)
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
chen::dns::rr_ninfo::rr_ninfo() : rr(TypeNINFO)
{
}

std::string chen::dns::rr_ninfo::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->zs_data;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_ninfo::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_ninfo::pack(dns::encoder &encoder) const
{
    encoder.pack(this->zs_data, codec::StringType::Plain, false);
}

void chen::dns::rr_ninfo::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->zs_data, codec::StringType::Plain);
}

void chen::dns::rr_ninfo::unpack(const json::object &object)
{
    map::write(object, "zs_data", this->zs_data);
}


// -----------------------------------------------------------------------------
// RKEY
chen::dns::rr_rkey::rr_rkey() : rr(TypeRKEY)
{
}

std::string chen::dns::rr_rkey::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_rkey::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_rkey::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::rr_rkey::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::rr_rkey::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// TALINK
chen::dns::rr_talink::rr_talink() : rr(TypeTALINK)
{
}

std::string chen::dns::rr_talink::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + this->previous_name;
    ret += " " + this->next_name;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_talink::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_talink::pack(dns::encoder &encoder) const
{
    encoder.pack(this->previous_name, codec::StringType::Domain, false);
    encoder.pack(this->next_name, codec::StringType::Domain, false);
}

void chen::dns::rr_talink::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->previous_name, codec::StringType::Domain);
    decoder.unpack(this->next_name, codec::StringType::Domain);
}

void chen::dns::rr_talink::unpack(const json::object &object)
{
    map::write(object, "previous_name", this->previous_name);
    map::write(object, "next_name", this->next_name);
}


// -----------------------------------------------------------------------------
// CDS
chen::dns::rr_cds::rr_cds() : rr(TypeCDS)
{
}

std::string chen::dns::rr_cds::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_cds::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_cds::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::rr_cds::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::rr_cds::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// CDNSKEY
chen::dns::rr_cdnskey::rr_cdnskey() : rr(TypeCDNSKEY)
{
}

std::string chen::dns::rr_cdnskey::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + num::str(this->protocol);
    ret += " " + num::str(this->algorithm);
    ret += " " + this->publickey;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_cdnskey::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_cdnskey::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->protocol);
    encoder.pack(this->algorithm);
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::rr_cdnskey::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->protocol);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::rr_cdnskey::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "protocol", this->protocol);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// OPENPGPKEY
chen::dns::rr_openpgpkey::rr_openpgpkey() : rr(TypeOPENPGPKEY)
{
}

std::string chen::dns::rr_openpgpkey::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + this->publickey;
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_openpgpkey::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_openpgpkey::pack(dns::encoder &encoder) const
{
    encoder.pack(this->publickey, codec::StringType::Plain, false);
}

void chen::dns::rr_openpgpkey::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->publickey, codec::StringType::Plain);
}

void chen::dns::rr_openpgpkey::unpack(const json::object &object)
{
    map::write(object, "publickey", this->publickey);
}


// -----------------------------------------------------------------------------
// CSYNC
chen::dns::rr_csync::rr_csync() : rr(TypeCSYNC)
{
}

std::string chen::dns::rr_csync::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->serial);
    ret += " " + num::str(this->flags);
    ret += " " + rr::escape(this->type_bitmap.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_csync::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_csync::pack(dns::encoder &encoder) const
{
    encoder.pack(this->serial);
    encoder.pack(this->flags);
    encoder.pack(this->type_bitmap, this->type_bitmap.size());
}

void chen::dns::rr_csync::unpack(dns::decoder &decoder)
{
    auto tmp = decoder.cur();

    decoder.unpack(this->serial);
    decoder.unpack(this->flags);

    this->type_bitmap.clear();
    decoder.unpack(this->type_bitmap, this->remain(static_cast<std::size_t>(std::distance(tmp, decoder.cur()))));
}

void chen::dns::rr_csync::unpack(const json::object &object)
{
    map::write(object, "serial", this->serial);
    map::write(object, "flags", this->flags);

    this->type_bitmap.clear();

    std::string type_bitmap = map::find(object, "type_bitmap", std::string());
    std::copy(type_bitmap.begin(), type_bitmap.end(), this->type_bitmap.begin());
}


// -----------------------------------------------------------------------------
// SPF
chen::dns::rr_spf::rr_spf() : rr(TypeSPF)
{
}

std::string chen::dns::rr_spf::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + rr::escape(this->txt);
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_spf::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_spf::pack(dns::encoder &encoder) const
{
    encoder.pack(this->txt, codec::StringType::Plain, false);
}

void chen::dns::rr_spf::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->txt, codec::StringType::Plain);
}

void chen::dns::rr_spf::unpack(const json::object &object)
{
    map::write(object, "txt", this->txt);
}


// -----------------------------------------------------------------------------
// UINFO
chen::dns::rr_uinfo::rr_uinfo() : raw(TypeUINFO)
{
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_uinfo::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// UID
chen::dns::rr_uid::rr_uid() : raw(TypeUID)
{
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_uid::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// GID
chen::dns::rr_gid::rr_gid() : raw(TypeGID)
{
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_gid::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// UNSPEC
chen::dns::rr_unspec::rr_unspec() : raw(TypeUNSPEC)
{
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_unspec::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}


// -----------------------------------------------------------------------------
// NID
chen::dns::rr_nid::rr_nid() : rr(TypeNID)
{
}

std::string chen::dns::rr_nid::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->node_id);

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_nid::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_nid::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->node_id);
}

void chen::dns::rr_nid::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->node_id);
}

void chen::dns::rr_nid::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "node_id", this->node_id);
}


// -----------------------------------------------------------------------------
// L32
chen::dns::rr_l32::rr_l32() : rr(TypeL32)
{
}

std::string chen::dns::rr_l32::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->locator32);

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_l32::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_l32::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator32);
}

void chen::dns::rr_l32::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->locator32);
}

void chen::dns::rr_l32::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "locator32", this->locator32);
}


// -----------------------------------------------------------------------------
// L64
chen::dns::rr_l64::rr_l64() : rr(TypeL64)
{
}

std::string chen::dns::rr_l64::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + num::str(this->locator64);

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_l64::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_l64::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->locator64);
}

void chen::dns::rr_l64::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->locator64);
}

void chen::dns::rr_l64::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "locator64", this->locator64);
}


// -----------------------------------------------------------------------------
// LP
chen::dns::rr_lp::rr_lp() : rr(TypeLP)
{
}

std::string chen::dns::rr_lp::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->preference);
    ret += " " + this->fqdn;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_lp::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_lp::pack(dns::encoder &encoder) const
{
    encoder.pack(this->preference);
    encoder.pack(this->fqdn, codec::StringType::Domain, false);
}

void chen::dns::rr_lp::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->preference);
    decoder.unpack(this->fqdn, codec::StringType::Domain);
}

void chen::dns::rr_lp::unpack(const json::object &object)
{
    map::write(object, "preference", this->preference);
    map::write(object, "fqdn", this->fqdn);
}


// -----------------------------------------------------------------------------
// EUI48
chen::dns::rr_eui48::rr_eui48() : rr(TypeEUI48)
{
}

std::string chen::dns::rr_eui48::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + rr::escape(this->address.size());
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_eui48::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_eui48::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::rr_eui48::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::rr_eui48::unpack(const json::object &object)
{
    std::string address = map::find(object, "address", std::string());
    std::copy(address.begin(), address.end(), this->address.begin());
}


// -----------------------------------------------------------------------------
// EUI64
chen::dns::rr_eui64::rr_eui64() : rr(TypeEUI64)
{
}

std::string chen::dns::rr_eui64::str(const std::string &sep) const
{
    auto ret = rr::str(sep);
    ret += sep + num::str(this->address);
    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_eui64::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_eui64::pack(dns::encoder &encoder) const
{
    encoder.pack(this->address);
}

void chen::dns::rr_eui64::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->address);
}

void chen::dns::rr_eui64::unpack(const json::object &object)
{
    map::write(object, "address", this->address);
}


// -----------------------------------------------------------------------------
// TKEY
chen::dns::rr_tkey::rr_tkey() : rr(TypeTKEY)
{
}

std::string chen::dns::rr_tkey::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + this->algorithm;
    ret += " " + num::str(this->inception);
    ret += " " + num::str(this->expiration);
    ret += " " + num::str(this->mode);
    ret += " " + num::str(this->error);
    ret += " " + num::str(this->key_size);
    ret += " " + rr::escape(this->key.size());
    ret += " " + num::str(this->other_len);
    ret += " " + rr::escape(this->other_data.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_tkey::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_tkey::pack(dns::encoder &encoder) const
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

void chen::dns::rr_tkey::unpack(dns::decoder &decoder)
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

void chen::dns::rr_tkey::unpack(const json::object &object)
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
chen::dns::rr_tsig::rr_tsig() : rr(TypeTSIG)
{
}

std::string chen::dns::rr_tsig::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + this->algorithm;
    ret += " " + rr::escape(this->time_signed.size());
    ret += " " + num::str(this->fudge);
    ret += " " + num::str(this->mac_size);
    ret += " " + rr::escape(this->mac.size());
    ret += " " + num::str(this->original_id);
    ret += " " + num::str(this->error);
    ret += " " + num::str(this->other_len);
    ret += " " + rr::escape(this->other_data.size());

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_tsig::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_tsig::pack(dns::encoder &encoder) const
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

void chen::dns::rr_tsig::unpack(dns::decoder &decoder)
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

void chen::dns::rr_tsig::unpack(const json::object &object)
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
chen::dns::rr_uri::rr_uri() : rr(TypeURI)
{
}

std::string chen::dns::rr_uri::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->priority);
    ret += " " + num::str(this->weight);
    ret += " " + this->target;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_uri::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_uri::pack(dns::encoder &encoder) const
{
    encoder.pack(this->priority);
    encoder.pack(this->weight);
    encoder.pack(this->target, codec::StringType::Plain, false);
}

void chen::dns::rr_uri::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->priority);
    decoder.unpack(this->weight);
    decoder.unpack(this->target, codec::StringType::Plain);
}

void chen::dns::rr_uri::unpack(const json::object &object)
{
    map::write(object, "priority", this->priority);
    map::write(object, "weight", this->weight);
    map::write(object, "target", this->target);
}


// -----------------------------------------------------------------------------
// CAA
chen::dns::rr_caa::rr_caa() : rr(TypeCAA)
{
}

std::string chen::dns::rr_caa::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->flags);
    ret += " " + this->tag;
    ret += " " + this->value;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_caa::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_caa::pack(dns::encoder &encoder) const
{
    encoder.pack(this->flags);
    encoder.pack(this->tag, codec::StringType::Plain, false);
    encoder.pack(this->value, codec::StringType::Plain, false);
}

void chen::dns::rr_caa::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->flags);
    decoder.unpack(this->tag, codec::StringType::Plain);
    decoder.unpack(this->value, codec::StringType::Plain);
}

void chen::dns::rr_caa::unpack(const json::object &object)
{
    map::write(object, "flags", this->flags);
    map::write(object, "tag", this->tag);
    map::write(object, "value", this->value);
}


// -----------------------------------------------------------------------------
// TA
chen::dns::rr_ta::rr_ta() : rr(TypeTA)
{
}

std::string chen::dns::rr_ta::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_ta::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_ta::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::rr_ta::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::rr_ta::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}


// -----------------------------------------------------------------------------
// DLV
chen::dns::rr_dlv::rr_dlv() : rr(TypeDLV)
{
}

std::string chen::dns::rr_dlv::str(const std::string &sep) const
{
    auto ret = rr::str(sep);

    ret += sep + num::str(this->key_tag);
    ret += " " + num::str(this->algorithm);
    ret += " " + num::str(this->digest_type);
    ret += " " + this->digest;

    return ret;
}

std::shared_ptr<chen::dns::rr> chen::dns::rr_dlv::clone() const
{
    return std::make_shared<typename std::decay<decltype(*this)>::type>(*this);
}

void chen::dns::rr_dlv::pack(dns::encoder &encoder) const
{
    encoder.pack(this->key_tag);
    encoder.pack(this->algorithm);
    encoder.pack(this->digest_type);
    encoder.pack(this->digest, codec::StringType::Plain, false);
}

void chen::dns::rr_dlv::unpack(dns::decoder &decoder)
{
    decoder.unpack(this->key_tag);
    decoder.unpack(this->algorithm);
    decoder.unpack(this->digest_type);
    decoder.unpack(this->digest, codec::StringType::Plain);
}

void chen::dns::rr_dlv::unpack(const json::object &object)
{
    map::write(object, "key_tag", this->key_tag);
    map::write(object, "algorithm", this->algorithm);
    map::write(object, "digest_type", this->digest_type);
    map::write(object, "digest", this->digest);
}