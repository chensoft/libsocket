/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "dns_packet.hpp"
#include "dns_record.hpp"
#include "dns_error.hpp"
#include "dns_codec.hpp"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// message
message::~message()
{

}

// property
const chen::dns::header& message::header() const
{
    return this->_header;
}

chen::dns::header& message::header()
{
    return this->_header;
}

void message::setHeader(const chen::dns::header &value)
{
    this->_header = value;
}

// codec
std::vector<std::uint8_t> message::encode() const
{
    return this->_header.encode();
}

void message::encode(std::vector<std::uint8_t> &out) const
{
    this->_header.encode(out);
}

void message::decode(const std::vector<std::uint8_t> &data)
{
    auto cur = data.begin();
    auto end = data.end();
    this->decode(cur, end);
}

void message::decode(std::vector<std::uint8_t>::const_iterator &cur,
                     std::vector<std::uint8_t>::const_iterator &end)
{
    this->_header.decode(cur, end);
}


// -----------------------------------------------------------------------------
// request

// question
void request::setQuestion(const std::string &qname,
                          chen::dns::RRType qtype,
                          chen::dns::RRClass qclass,
                          std::uint16_t id,
                          bool recursionDesired)
{
    // check empty
    if (qname.empty())
        throw error_size("dns: request query name is empty");

    // check fqdn
    if (!codec::isFqdn(qname))
        throw error_fqdn("dns: request query name is not fqdn");

    // set id
    this->_header.setId(id);

    // set qr
    this->_header.setQr(QR::Query);

    // set opcode
    this->_header.setOpcode(chen::dns::OPCODE::Query);

    // set qdcount
    this->_header.setQdcount(1);

    // set recursion desired
    this->_header.setRecursionDesired(recursionDesired);

    // set question
    this->_question.setQname(qname);
    this->_question.setQtype(qtype);
    this->_question.setQclass(qclass);
}

// get
const chen::dns::question& request::question() const
{
    return this->_question;
}

chen::dns::question& request::question()
{
    return this->_question;
}

// set
void request::setQuestion(const chen::dns::question &value)
{
    this->_question = value;
}

// address
std::string request::addr() const
{
    return this->_addr;
}

std::uint16_t request::port() const
{
    return this->_port;
}

void request::setAddr(const std::string &value)
{
    this->_addr = value;
}

void request::setPort(std::uint16_t value)
{
    this->_port = value;
}

// codec
std::vector<std::uint8_t> request::encode() const
{
    std::vector<std::uint8_t> out;
    this->encode(out);
    return out;
}

void request::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // header
        message::encode(out);

        // question
        this->_question.encode(out);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void request::decode(std::vector<std::uint8_t>::const_iterator &cur,
                     std::vector<std::uint8_t>::const_iterator &end)
{
    message::decode(cur, end);
    this->_question.decode(cur, end);
}


// -----------------------------------------------------------------------------
// response
response::response()
{
    this->_header.setQr(chen::dns::QR::Response);
    this->_header.setRcode(chen::dns::RCODE::NoError);
}

response::response(const response &o)
{
    *this = o;
}

response& response::operator=(const response &o)
{
    if (this == &o)
        return *this;

    this->_question   = o._question;
    this->_answer     = o._answer;
    this->_authority  = o._authority;
    this->_additional = o._additional;

    return *this;
}

response::response(response &&o)
{
    *this = std::move(o);
}

response& response::operator=(response &&o)
{
    if (this == &o)
        return *this;

    this->_question   = std::move(o._question);
    this->_answer     = std::move(o._answer);
    this->_authority  = std::move(o._authority);
    this->_additional = std::move(o._additional);

    return *this;
}

// add
void response::addQuestion(const chen::dns::request &value)
{
    this->_header.setId(value.header().id());
    this->addQuestion(value.question());
}

void response::addQuestion(const chen::dns::question &value)
{
    this->_question.push_back(value);
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAnswer(std::shared_ptr<chen::dns::RR> value)
{
    this->_answer.push_back(std::move(value));
    this->_header.setAncount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAuthority(std::shared_ptr<chen::dns::RR> value)
{
    this->_authority.push_back(std::move(value));
    this->_header.setNscount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAdditional(std::shared_ptr<chen::dns::RR> value)
{
    this->_additional.push_back(std::move(value));
    this->_header.setArcount(static_cast<std::uint16_t>(this->_question.size()));
}

// get
const response::q_type& response::question() const
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

// set
void response::setQuestion(const q_type &value)
{
    this->_question = value;
    this->_header.setQdcount(static_cast<std::uint16_t>(value.size()));
}

void response::setAnswer(const rr_type &value)
{
    this->_answer = value;
    this->_header.setAncount(static_cast<std::uint16_t>(value.size()));
}

void response::setAuthority(const rr_type &value)
{
    this->_authority = value;
    this->_header.setNscount(static_cast<std::uint16_t>(value.size()));
}

void response::setAdditional(const rr_type &value)
{
    this->_additional = value;
    this->_header.setArcount(static_cast<std::uint16_t>(value.size()));
}

// codec
std::vector<std::uint8_t> response::encode() const
{
    std::vector<std::uint8_t> out;
    this->encode(out);
    return out;
}

void response::encode(std::vector<std::uint8_t> &out) const
{
    auto size = out.size();

    try
    {
        // header
        message::encode(out);

        // question
        for (auto &val : this->_question)
            val.encode(out);

        // answer
        for (auto &val : this->_answer)
            val->encode(out);

        // authority
        for (auto &val : this->_authority)
            val->encode(out);

        // additional
        for (auto &val : this->_additional)
            val->encode(out);
    }
    catch (...)
    {
        // restore
        out.erase(out.begin() + size, out.end());
        throw;
    }
}

void response::decode(std::vector<std::uint8_t>::const_iterator &cur,
                      std::vector<std::uint8_t>::const_iterator &end)
{
    // header
    message::decode(cur, end);

    // question
    this->_question.clear();

    for (std::uint16_t i = 0, len = this->_header.qdcount(); i < len; ++i)
    {
        chen::dns::question q;
        q.decode(cur, end);

        this->_question.push_back(q);
    }

    // answer
    this->_answer.clear();

    for (std::uint16_t i = 0, len = this->_header.ancount(); i < len; ++i)
    {
        this->_answer.push_back(chen::dns::RR::decode(cur, end));
    }

    // authority
    this->_authority.clear();

    for (std::uint16_t i = 0, len = this->_header.nscount(); i < len; ++i)
    {
        this->_authority.push_back(chen::dns::RR::decode(cur, end));
    }

    // additional
    this->_additional.clear();

    for (std::uint16_t i = 0, len = this->_header.arcount(); i < len; ++i)
    {
        this->_additional.push_back(chen::dns::RR::decode(cur, end));
    }
}