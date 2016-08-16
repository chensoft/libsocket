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

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// header

// filed value
std::uint16_t header::id() const
{
    return this->_id;
}

std::uint16_t header::flag() const
{
    return this->_flag;
}

std::uint16_t header::qdcount() const
{
    return this->_qdcount;
}

std::uint16_t header::ancount() const
{
    return this->_ancount;
}

std::uint16_t header::nscount() const
{
    return this->_nscount;
}

std::uint16_t header::arcount() const
{
    return this->_arcount;
}

// set filed value
void header::setId(std::uint16_t value)
{
    this->_id = value;
}

void header::setFlag(std::uint16_t value)
{
    this->_flag = value;
}

void header::setQdcount(std::uint16_t value)
{
    this->_qdcount = value;
}

void header::setAncount(std::uint16_t value)
{
    this->_ancount = value;
}

void header::setNscount(std::uint16_t value)
{
    this->_nscount = value;
}

void header::setArcount(std::uint16_t value)
{
    this->_arcount = value;
}

// flag value
QR header::qr() const
{
    return static_cast<QR>(this->_flag & FLAG_MASK_QR);
}

OPCODE header::opcode() const
{
    return static_cast<OPCODE>(this->_flag & FLAG_MASK_OPCODE);
}

bool header::authoritative() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_AA);
}

bool header::truncated() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_TC);
}

bool header::recursionDesired() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_RD);
}

bool header::recursionAvailable() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_RA);
}

bool header::zero() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_Z);
}

bool header::authenticData() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_AD);
}

bool header::checkingDisabled() const
{
    return static_cast<bool>(this->_flag & FLAG_MASK_CD);
}

RCODE header::rcode() const
{
    return static_cast<RCODE>(this->_flag & FLAG_MASK_RCODE);
}

// set flag value
void header::setQr(QR value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_QR) | ((static_cast<std::uint16_t>(value) << FLAG_POS_QR) & FLAG_MASK_QR);
}

void header::setOpcode(OPCODE value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_OPCODE) | ((static_cast<std::uint16_t>(value) << FLAG_POS_OPCODE) & FLAG_MASK_OPCODE);
}

void header::setAuthoritative(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_AA) | ((static_cast<std::uint16_t>(value) << FLAG_POS_AA) & FLAG_MASK_AA);
}

void header::setTruncated(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_TC) | ((static_cast<std::uint16_t>(value) << FLAG_POS_TC) & FLAG_MASK_TC);
}

void header::setRecursionDesired(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RD) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RD) & FLAG_MASK_RD);
}

void header::setRecursionAvailable(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RA) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RA) & FLAG_MASK_RA);
}

void header::setZero(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_Z) | ((static_cast<std::uint16_t>(value) << FLAG_POS_Z) & FLAG_MASK_Z);
}

void header::setAuthenticData(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_AD) | ((static_cast<std::uint16_t>(value) << FLAG_POS_AD) & FLAG_MASK_AD);
}

void header::setCheckingDisabled(bool value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_CD) | ((static_cast<std::uint16_t>(value) << FLAG_POS_CD) & FLAG_MASK_CD);
}

void header::setRcode(RCODE value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RCODE) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RCODE) & FLAG_MASK_RCODE);
}

// codec
void header::encode(dns::encoder &encoder) const
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

void header::decode(dns::decoder &decoder)
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
std::uint16_t header::random()
{
    return static_cast<std::uint16_t>(num::random(0, 0xFFFF));
}


// -----------------------------------------------------------------------------
// question
question::question(const std::string &qname,
                   RRType qtype,
                   RRClass qclass) : _qname(qname), _qtype(qtype), _qclass(qclass)
{
}

// get filed value
const std::string& question::qname() const
{
    return this->_qname;
}

RRType question::qtype() const
{
    return this->_qtype;
}

RRClass question::qclass() const
{
    return this->_qclass;
}

// set field value
void question::setQname(const std::string &value)
{
    this->_qname = value;
}

void question::setQname(std::string &&value)
{
    this->_qname = std::move(value);
}

void question::setQtype(RRType value)
{
    this->_qtype = value;
}

void question::setQclass(RRClass value)
{
    this->_qclass = value;
}

// codec
void question::encode(dns::encoder &encoder) const
{
    // qname
    encoder.pack(this->_qname, codec::StringType::Domain, false);

    // qtype
    encoder.pack(this->_qtype);

    // qclass
    encoder.pack(this->_qclass);
}

void question::decode(dns::decoder &decoder)
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
std::string question::str(const std::string &sep) const
{
    std::string ret;

    ret += this->_qname + sep;
    ret += table::classToText(this->_qclass) + sep;
    ret += table::typeToText(this->_qtype);

    return ret;
}