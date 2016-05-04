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

void message::decode(const std::vector<std::uint8_t> &data)
{
    this->_header.decode(data);
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
    auto ret = message::encode();
    auto tmp = this->_question.encode();

    ret.insert(ret.end(), tmp.begin(), tmp.end());
    return ret;
}

void request::decode(const std::vector<std::uint8_t> &data)
{
    // todo avoid copy
    message::decode(data);
    this->_question.decode(std::vector<std::uint8_t>(data.begin() + 0, data.end()));  // todo add correct bytes
}


// -----------------------------------------------------------------------------
// response
response::response(std::uint16_t id)
{
    this->_header.setId(id);
    this->_header.setQr(chen::dns::QR::Response);
}

response::response(const response &o)
{
    *this = o;
}

response& response::operator=(const response &o)
{
    if (this == &o)
        return *this;

    // todo base class header member?
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

    // todo base class header member?
    this->_question   = std::move(o._question);
    this->_answer     = std::move(o._answer);
    this->_authority  = std::move(o._authority);
    this->_additional = std::move(o._additional);

    return *this;
}

// add
void response::addQuestion(const chen::dns::question &value)
{
    this->_question.push_back(value);
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAnswer(std::shared_ptr<chen::dns::RR> value)
{
    this->_answer.push_back(value);
    this->_header.setAncount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAuthority(std::shared_ptr<chen::dns::RR> value)
{
    this->_authority.push_back(value);
    this->_header.setNscount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAdditional(std::shared_ptr<chen::dns::RR> value)
{
    this->_additional.push_back(value);
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
    // todo avoid copy
    // header
    auto ret = message::encode();

    // question
    for (auto &val : this->_question)
    {
        auto tmp = val.encode();
        ret.insert(ret.end(), tmp.begin(), tmp.end());
    }

    // answer
    for (auto &val : this->_answer)
    {
        auto tmp = val->encode();
        ret.insert(ret.end(), tmp.begin(), tmp.end());
    }

    // authority
    for (auto &val : this->_authority)
    {
        auto tmp = val->encode();
        ret.insert(ret.end(), tmp.begin(), tmp.end());
    }

    // additional
    for (auto &val : this->_additional)
    {
        auto tmp = val->encode();
        ret.insert(ret.end(), tmp.begin(), tmp.end());
    }

    return ret;
}

void response::decode(const std::vector<std::uint8_t> &data)
{
    // header
    message::decode(data);

    // question
    this->_question.clear();

    for (std::uint16_t i = 0, len = this->_header.qdcount(); i < len; ++i)
    {
        chen::dns::question q;
        q.decode(std::vector<std::uint8_t>(data.begin(), data.end()));  // todo add bytes

        this->_question.push_back(q);
    }

    // answer
    this->_answer.clear();

    for (std::uint16_t i = 0, len = this->_header.ancount(); i < len; ++i)
    {
        this->_answer.push_back(chen::dns::RR::decode(std::vector<std::uint8_t>(data.begin(), data.end())));  // todo add bytes
    }

    // authority
    this->_authority.clear();

    for (std::uint16_t i = 0, len = this->_header.nscount(); i < len; ++i)
    {
        this->_authority.push_back(chen::dns::RR::decode(std::vector<std::uint8_t>(data.begin(), data.end())));  // todo add bytes
    }

    // additional
    this->_additional.clear();

    for (std::uint16_t i = 0, len = this->_header.arcount(); i < len; ++i)
    {
        this->_additional.push_back(chen::dns::RR::decode(std::vector<std::uint8_t>(data.begin(), data.end())));  // todo add bytes
    }
}