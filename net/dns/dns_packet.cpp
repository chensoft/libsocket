/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_packet.h"
#include "dns_error.h"
#include "dns_tool.h"
#include "tool/log.h"
#include <random>
#include <chrono>

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

// flag value
chen::dns::QR header::qr() const
{
    return static_cast<chen::dns::QR>(this->_flag & 0b1000000000000000);
}

chen::dns::OPCODE header::opcode() const
{
    return static_cast<chen::dns::OPCODE>(this->_flag & 0b0111100000000000);
}

chen::dns::AA header::aa() const
{
    return static_cast<chen::dns::AA>(this->_flag & 0b0000010000000000);
}

chen::dns::TC header::tc() const
{
    return static_cast<chen::dns::TC>(this->_flag & 0b0000001000000000);
}

chen::dns::RD header::rd() const
{
    return static_cast<chen::dns::RD>(this->_flag & 0b0000000100000000);
}

chen::dns::RA header::ra() const
{
    return static_cast<chen::dns::RA>(this->_flag & 0b0000000010000000);
}

chen::dns::Z header::z() const
{
    return static_cast<chen::dns::Z>(this->_flag & 0b0000000001110000);
}

chen::dns::RCODE header::rcode() const
{
    return static_cast<chen::dns::RCODE>(this->_flag & 0b0000000000001111);
}

// set filed value
void header::setId(std::uint16_t value)
{
    this->_id = value ? value : header::random();
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

// set flag value
void header::setQr(chen::dns::QR value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 15) & static_cast<std::uint16_t>(0b1000000000000000);
    this->_flag |= tmp;
}

void header::setOpcode(chen::dns::OPCODE value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 11) & static_cast<std::uint16_t>(0b0111100000000000);
    this->_flag |= tmp;
}

void header::setAa(chen::dns::AA value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 10) & static_cast<std::uint16_t>(0b0000010000000000);
    this->_flag |= tmp;
}

void header::setTc(chen::dns::TC value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 9) & static_cast<std::uint16_t>(0b0000001000000000);
    this->_flag |= tmp;
}

void header::setRd(chen::dns::RD value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 8) & static_cast<std::uint16_t>(0b0000000100000000);
    this->_flag |= tmp;
}

void header::setRa(chen::dns::RA value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 7) & static_cast<std::uint16_t>(0b0000000010000000);
    this->_flag |= tmp;
}

void header::setZ(chen::dns::Z value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 4) & static_cast<std::uint16_t>(0b0000000001110000);
    this->_flag |= tmp;
}

void header::setRcode(chen::dns::RCODE value)
{
    std::uint16_t tmp = (static_cast<std::uint16_t>(value) << 0) & static_cast<std::uint16_t>(0b0000000000001111);
    this->_flag |= tmp;
}

// assign
void header::assign(const std::uint8_t *data, std::size_t size)
{
    if (size < 12)
        throw error_size("header assign size must be 12");

    this->_id      = (static_cast<std::uint16_t>(data[0]) << 8) | static_cast<std::uint16_t>(data[1]);
    this->_flag    = (static_cast<std::uint16_t>(data[2]) << 8) | static_cast<std::uint16_t>(data[3]);
    this->_qdcount = (static_cast<std::uint16_t>(data[4]) << 8) | static_cast<std::uint16_t>(data[5]);
    this->_ancount = (static_cast<std::uint16_t>(data[6]) << 8) | static_cast<std::uint16_t>(data[7]);
    this->_nscount = (static_cast<std::uint16_t>(data[8]) << 8) | static_cast<std::uint16_t>(data[9]);
    this->_arcount = (static_cast<std::uint16_t>(data[10]) << 8) | static_cast<std::uint16_t>(data[11]);
}

// binary
std::vector<std::uint8_t> header::binary() const
{
    std::vector<std::uint8_t> store;
    this->binary(store);
    return store;
}

void header::binary(std::vector<std::uint8_t> &store) const
{
    store.push_back(static_cast<std::uint8_t>(this->_id >> 8));
    store.push_back(static_cast<std::uint8_t>(this->_id));

    store.push_back(static_cast<std::uint8_t>(this->_flag >> 8));
    store.push_back(static_cast<std::uint8_t>(this->_flag));

    store.push_back(static_cast<std::uint8_t>(this->_qdcount >> 8));
    store.push_back(static_cast<std::uint8_t>(this->_qdcount));

    store.push_back(static_cast<std::uint8_t>(this->_ancount >> 8));
    store.push_back(static_cast<std::uint8_t>(this->_ancount));

    store.push_back(static_cast<std::uint8_t>(this->_nscount >> 8));
    store.push_back(static_cast<std::uint8_t>(this->_nscount));

    store.push_back(static_cast<std::uint8_t>(this->_arcount >> 8));
    store.push_back(static_cast<std::uint8_t>(this->_arcount));
}

// random
std::uint16_t header::random()
{
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<std::uint16_t> uniform(1, 0xFFFF);

    auto high = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return uniform(engine) ^ static_cast<uint16_t>(high);
}


// -----------------------------------------------------------------------------
// packet
void packet::packDomain(const std::string &name, std::vector<std::uint8_t> &store)
{
    // Note:
    // assume name is valid
    // each label is split by dot
    // label count + label value(exclude dot)
    std::vector<std::uint8_t>::size_type count = 0;
    std::vector<std::uint8_t>::size_type begin = store.size();

    store.push_back(0);  // size for next label

    for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(name.size()); i < len; ++i)
    {
        char c = name[i];

        if (c == '.')
        {
            store[store.size() - count - 1] = static_cast<std::uint8_t>(count);
            store.push_back(0);  // size for next label

            count = 0;
        }
        else
        {
            ++count;

            store.push_back(static_cast<std::uint8_t>(c));
        }
    }
}


// -----------------------------------------------------------------------------
// request
request::request()
{
    // use random id
    this->_qheader.setId();

    // set query
    this->_qheader.setQr(QR::Query);
}

// field value
header request::qheader() const
{
    return this->_qheader;
}

std::string request::qname() const
{
    return this->_qname;
}

chen::dns::RRType request::qtype() const
{
    return this->_qtype;
}

chen::dns::RRClass request::qclass() const
{
    return this->_qclass;
}

// set query
void request::setQuery(const std::string &qname,
                        chen::dns::RRType qtype,
                        chen::dns::RRClass qclass)
{
    // check empty
    if (qname.empty())
        throw error_size("request query name is empty");

    // check fqdn
    if (!tool::isFqdn(qname))
        throw error_fqdn("request query name is not fqdn");

    // check total length
    if (qname.size() > 255)
        throw error_size("request query name length must be 255 or less");

    // check each label
    std::string::size_type count = 0;

    for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(qname.size()); i < len; ++i)
    {
        char c = qname[i];

        if (c == '.')
        {
            if (!count)
                throw error_size("request query label is empty");

            count = 0;
        }
        else
        {
            ++count;

            if (count > 63)
                throw error_size("request query label length must be 63 or less");
        }
    }

    // set opcode
    this->_qheader.setOpcode(chen::dns::OPCODE::Query);

    // set qdcount
    this->_qheader.setQdcount(1);

    // set query
    this->_qname  = qname;
    this->_qtype  = qtype;
    this->_qclass = qclass;
}

// flag
void request::setRecursionDesired()
{
    this->_qheader.setRd(chen::dns::RD::Yes);
}

// binary
std::vector<std::uint8_t> request::binary() const
{
    std::vector<std::uint8_t> store;
    this->binary(store);
    return store;
}

void request::binary(std::vector<std::uint8_t> &store) const
{
    // header
    this->_qheader.binary(store);

    // name
    packet::packDomain(this->_qname, store);

    // type
    std::uint16_t qtype = static_cast<std::uint16_t>(this->_qtype);

    store.push_back(static_cast<std::uint8_t>(qtype >> 8));
    store.push_back(static_cast<std::uint8_t>(qtype));

    // class
    std::uint16_t qclass = static_cast<std::uint16_t>(this->_qclass);

    store.push_back(static_cast<std::uint8_t>(qclass >> 8));
    store.push_back(static_cast<std::uint8_t>(qclass));
}


// -----------------------------------------------------------------------------
// response

// rrs
const response::rr_type& response::question() const
{
    return this->_question;
}

const response::rr_type& response::answer() const
{
    return this->_answer;
}

const response::rr_type& response::authority() const
{
    return this->_authority;
}

const response::rr_type& response::additional() const
{
    return this->_additional;
}

// assign
void response::assign(const std::uint8_t *data, std::size_t size)
{
    this->_header.assign(data, size);

    // todo
    PILogE("haha: %s", tool::format(this->_header.binary()).c_str());
}