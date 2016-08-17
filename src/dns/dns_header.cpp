/**
 * Created by Jian Chen
 * @since  2015.11.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_header.hpp>
#include <socket/dns/dns_table.hpp>
#include <socket/dns/dns_codec.hpp>
#include <chen/base/num.hpp>

// -----------------------------------------------------------------------------
// header

// filed value
std::uint16_t chen::dns::header::id() const
{
    return this->_id;
}

std::uint16_t chen::dns::header::flag() const
{
    return this->_flag;
}

std::uint16_t chen::dns::header::qdcount() const
{
    return this->_qdcount;
}

std::uint16_t chen::dns::header::ancount() const
{
    return this->_ancount;
}

std::uint16_t chen::dns::header::nscount() const
{
    return this->_nscount;
}

std::uint16_t chen::dns::header::arcount() const
{
    return this->_arcount;
}

// set filed value
void chen::dns::header::setId(std::uint16_t value)
{
    this->_id = value;
}

void chen::dns::header::setFlag(std::uint16_t value)
{
    this->_flag = value;
}

void chen::dns::header::setQdcount(std::uint16_t value)
{
    this->_qdcount = value;
}

void chen::dns::header::setAncount(std::uint16_t value)
{
    this->_ancount = value;
}

void chen::dns::header::setNscount(std::uint16_t value)
{
    this->_nscount = value;
}

void chen::dns::header::setArcount(std::uint16_t value)
{
    this->_arcount = value;
}

// flag value
chen::dns::QR chen::dns::header::qr() const
{
    return static_cast<QR>(this->_flag & FLAG_MASK_QR);
}

chen::dns::OPCODE chen::dns::header::opcode() const
{
    return static_cast<OPCODE>(this->_flag & FLAG_MASK_OPCODE);
}

bool chen::dns::header::authoritative() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_AA);
}

bool chen::dns::header::truncated() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_TC);
}

bool chen::dns::header::recursionDesired() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_RD);
}

bool chen::dns::header::recursionAvailable() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_RA);
}

bool chen::dns::header::zero() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_Z);
}

bool chen::dns::header::authenticData() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_AD);
}

bool chen::dns::header::checkingDisabled() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_CD);
}

chen::dns::RCODE chen::dns::header::rcode() const
{
    return static_cast<RCODE>(this->_flag & FLAG_MASK_RCODE);
}

// set flag value
void chen::dns::header::setQr(QR value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_QR) | ((static_cast<std::uint16_t>(value) << FLAG_POS_QR) & FLAG_MASK_QR);
}

void chen::dns::header::setOpcode(OPCODE value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_OPCODE) | ((static_cast<std::uint16_t>(value) << FLAG_POS_OPCODE) & FLAG_MASK_OPCODE);
}

void chen::dns::header::setAuthoritative(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_AA) | ((static_cast<std::uint16_t>(value) << FLAG_POS_AA) & FLAG_MASK_AA);
}

void chen::dns::header::setTruncated(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_TC) | ((static_cast<std::uint16_t>(value) << FLAG_POS_TC) & FLAG_MASK_TC);
}

void chen::dns::header::setRecursionDesired(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RD) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RD) & FLAG_MASK_RD);
}

void chen::dns::header::setRecursionAvailable(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RA) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RA) & FLAG_MASK_RA);
}

void chen::dns::header::setZero(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_Z) | ((static_cast<std::uint16_t>(value) << FLAG_POS_Z) & FLAG_MASK_Z);
}

void chen::dns::header::setAuthenticData(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_AD) | ((static_cast<std::uint16_t>(value) << FLAG_POS_AD) & FLAG_MASK_AD);
}

void chen::dns::header::setCheckingDisabled(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_CD) | ((static_cast<std::uint16_t>(value) << FLAG_POS_CD) & FLAG_MASK_CD);
}

void chen::dns::header::setRcode(RCODE value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RCODE) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RCODE) & FLAG_MASK_RCODE);
}

// codec
void chen::dns::header::encode(dns::encoder &encoder) const
{
    // id
    encoder.pack(this->_id);

    // flag
    encoder.pack(this->_flag);

    // question
    encoder.pack(this->_qdcount);

    // answer
    encoder.pack(this->_ancount);

    // authority
    encoder.pack(this->_nscount);

    // additional
    encoder.pack(this->_arcount);
}

void chen::dns::header::decode(dns::decoder &decoder)
{
    // id
    std::uint16_t id = 0;
    decoder.unpack(id);

    // flag
    std::uint16_t flag = 0;
    decoder.unpack(flag);

    // question
    std::uint16_t qdcount = 0;
    decoder.unpack(qdcount);

    // answer
    std::uint16_t ancount = 0;
    decoder.unpack(ancount);

    // authority
    std::uint16_t nscount = 0;
    decoder.unpack(nscount);

    // additional
    std::uint16_t arcount = 0;
    decoder.unpack(arcount);

    // set
    this->_id      = id;
    this->_flag    = flag;
    this->_qdcount = qdcount;
    this->_ancount = ancount;
    this->_nscount = nscount;
    this->_arcount = arcount;
}

// random
std::uint16_t chen::dns::header::random()
{
    return static_cast<std::uint16_t>(num::random(0, 0xFFFF));
}


// -----------------------------------------------------------------------------
// question
chen::dns::question::question(const std::string &qname,
                              RRType qtype,
                              RRClass qclass) : _qname(qname), _qtype(qtype), _qclass(qclass)
{
}

// get filed value
const std::string& chen::dns::question::qname() const
{
    return this->_qname;
}

chen::dns::RRType chen::dns::question::qtype() const
{
    return this->_qtype;
}

chen::dns::RRClass chen::dns::question::qclass() const
{
    return this->_qclass;
}

// set field value
void chen::dns::question::setQname(const std::string &value)
{
    this->_qname = value;
}

void chen::dns::question::setQname(std::string &&value)
{
    this->_qname = std::move(value);
}

void chen::dns::question::setQtype(RRType value)
{
    this->_qtype = value;
}

void chen::dns::question::setQclass(RRClass value)
{
    this->_qclass = value;
}

// codec
void chen::dns::question::encode(dns::encoder &encoder) const
{
    // qname
    encoder.pack(this->_qname, codec::StringType::Domain, false);

    // qtype
    encoder.pack(this->_qtype);

    // qclass
    encoder.pack(this->_qclass);
}

void chen::dns::question::decode(dns::decoder &decoder)
{
    // qname
    std::string qname;
    decoder.unpack(qname, codec::StringType::Domain);

    // qtype
    RRType qtype = RRType::None;
    decoder.unpack(qtype);

    // qclass
    RRClass qclass = RRClass::IN;
    decoder.unpack(qclass);

    // set
    this->_qname  = std::move(qname);
    this->_qtype  = qtype;
    this->_qclass = qclass;
}

// desc
std::string chen::dns::question::str(const std::string &sep) const
{
    std::string ret;

    ret += this->_qname + sep;
    ret += table::classToText(this->_qclass) + sep;
    ret += table::typeToText(this->_qtype);

    return ret;
}