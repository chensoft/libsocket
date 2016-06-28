/**
 * Created by Jian Chen
 * @since  2015.11.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_header.hpp>
#include <socket/dns/dns_codec.hpp>
#include <socket/dns/dns_table.hpp>
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
chen::dns::QR header::qr() const
{
    return static_cast<chen::dns::QR>(this->_flag & FLAG_MASK_QR);
}

chen::dns::OPCODE header::opcode() const
{
    return static_cast<chen::dns::OPCODE>(this->_flag & FLAG_MASK_OPCODE);
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

chen::dns::RCODE header::rcode() const
{
    return static_cast<chen::dns::RCODE>(this->_flag & FLAG_MASK_RCODE);
}

// set flag value
void header::setQr(chen::dns::QR value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_QR) | ((static_cast<std::uint16_t>(value) << FLAG_POS_QR) & FLAG_MASK_QR);
}

void header::setOpcode(chen::dns::OPCODE value)
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


void header::setRcode(chen::dns::RCODE value)
{
    this->_flag = (this->_flag & ~FLAG_MASK_RCODE) | ((static_cast<std::uint16_t>(value) << FLAG_POS_RCODE) & FLAG_MASK_RCODE);
}

// codec
std::vector<std::uint8_t> header::encode() const
{
    std::vector<std::uint8_t> out;
    this->encode(out);
    return out;
}

void header::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // id
        encoder::pack(this->_id, out);

        // flag
        encoder::pack(this->_flag, out);

        // question
        encoder::pack(this->_qdcount, out);

        // answer
        encoder::pack(this->_ancount, out);

        // authority
        encoder::pack(this->_nscount, out);

        // additional
        encoder::pack(this->_arcount, out);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void header::decode(const std::vector<std::uint8_t> &data)
{
    auto cur = data.begin();
    auto end = data.end();
    this->decode(cur, end);
}

void header::decode(std::vector<std::uint8_t>::const_iterator &cur,
                    std::vector<std::uint8_t>::const_iterator &end)
{
    // id
    std::uint16_t id = 0;
    decoder::unpack(id, cur, end);

    // flag
    std::uint16_t flag = 0;
    decoder::unpack(flag, cur, end);

    // question
    std::uint16_t qdcount = 0;
    decoder::unpack(qdcount, cur, end);

    // answer
    std::uint16_t ancount = 0;
    decoder::unpack(ancount, cur, end);

    // authority
    std::uint16_t nscount = 0;
    decoder::unpack(nscount, cur, end);

    // additional
    std::uint16_t arcount = 0;
    decoder::unpack(arcount, cur, end);

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
                   chen::dns::RRType qtype,
                   chen::dns::RRClass qclass)
: _qname(qname)
, _qtype(qtype)
, _qclass(qclass)
{

}

// get filed value
const std::string& question::qname() const
{
    return this->_qname;
}

chen::dns::RRType question::qtype() const
{
    return this->_qtype;
}

chen::dns::RRClass question::qclass() const
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

void question::setQtype(chen::dns::RRType value)
{
    this->_qtype = value;
}

void question::setQclass(chen::dns::RRClass value)
{
    this->_qclass = value;
}

// codec
std::vector<std::uint8_t> question::encode() const
{
    std::vector<std::uint8_t> out;
    this->encode(out);
    return out;
}

void question::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // qname
        encoder::pack(this->_qname, true, out);

        // qtype
        encoder::pack(this->_qtype, out);

        // qclass
        encoder::pack(this->_qclass, out);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void question::decode(const std::vector<std::uint8_t> &data)
{
    auto cur = data.begin();
    auto end = data.end();
    this->decode(cur, end);
}

void question::decode(std::vector<std::uint8_t>::const_iterator &cur,
                      std::vector<std::uint8_t>::const_iterator &end)
{
    // qname
    std::string qname;
    decoder::unpack(qname, true, cur, end);

    // qtype
    chen::dns::RRType qtype = chen::dns::RRType::None;
    decoder::unpack(qtype, cur, end);

    // qclass
    chen::dns::RRClass qclass = chen::dns::RRClass::IN;
    decoder::unpack(qclass, cur, end);

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
    ret += chen::dns::table::classToText(this->_qclass) + sep;
    ret += chen::dns::table::typeToText(this->_qtype);

    return ret;
}