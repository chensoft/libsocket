/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_packet.hpp>
#include <socket/dns/dns_record.hpp>

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
void message::encode(chen::dns::encoder &encoder) const
{
    this->_header.encode(encoder);
}

void message::decode(chen::dns::decoder &decoder)
{
    this->_header.decode(decoder);
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
        throw error_codec("dns: request query name is empty");

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

// question
const chen::dns::question& request::question() const
{
    return this->_question;
}

chen::dns::question& request::question()
{
    return this->_question;
}

void request::setQuestion(const chen::dns::question &value)
{
    this->_question = value;
}

void request::setQuestion(chen::dns::question &&value)
{
    this->_question = std::move(value);
}

// client
const std::string& request::addr() const
{
    return this->_addr;
}

std::uint16_t request::port() const
{
    return this->_port;
}

void request::setAddr(const std::string &addr)
{
    this->_addr = addr;
}

void request::setPort(std::uint16_t port)
{
    this->_port = port;
}

// codec
std::vector<std::uint8_t> request::encode() const
{
    chen::dns::encoder encoder;
    this->encode(encoder);
    return encoder.data();
}

void request::decode(chen::dns::codec::iterator beg, chen::dns::codec::iterator end,
                     const std::string &addr, std::uint16_t port)
{
    chen::dns::decoder decoder(beg, end);
    this->decode(decoder);

    this->setAddr(addr);
    this->setPort(port);
}

void request::encode(chen::dns::encoder &encoder) const
{
    // header
    message::encode(encoder);

    // question
    this->_question.encode(encoder);
}

void request::decode(chen::dns::decoder &decoder)
{
    message::decode(decoder);
    this->_question.decode(decoder);
}


// -----------------------------------------------------------------------------
// response
response::response(bool authoritative)
{
    this->_header.setQr(chen::dns::QR::Response);
    this->_header.setOpcode(chen::dns::OPCODE::Query);
    this->_header.setAuthoritative(authoritative);
    this->_header.setRcode(chen::dns::RCODE::NoError);
}

// add
void response::addQuestion(const chen::dns::question &value)
{
    this->_question.emplace_back(value);
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addQuestion(chen::dns::question &&value)
{
    this->_question.emplace_back(std::move(value));
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::addAnswer(std::shared_ptr<chen::dns::RR> value)
{
    this->_answer.emplace_back(value);
    this->_header.setAncount(static_cast<std::uint16_t>(this->_answer.size()));
}

void response::addAuthority(std::shared_ptr<chen::dns::RR> value)
{
    this->_authority.emplace_back(value);
    this->_header.setNscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void response::addAdditional(std::shared_ptr<chen::dns::RR> value)
{
    this->_additional.emplace_back(value);
    this->_header.setArcount(static_cast<std::uint16_t>(this->_additional.size()));
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
void response::setQuestion(const chen::dns::request &value)
{
    this->_header.setId(value.header().id());
    this->_header.setRecursionDesired(value.header().recursionDesired());

    this->_question.clear();
    this->addQuestion(value.question());
}

void response::setQuestion(const q_type &value)
{
    this->_question = value;
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::setAnswer(const rr_type &value)
{
    this->_answer = value;
    this->_header.setAncount(static_cast<std::uint16_t>(this->_answer.size()));
}

void response::setAuthority(const rr_type &value)
{
    this->_authority = value;
    this->_header.setNscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void response::setAdditional(const rr_type &value)
{
    this->_additional = value;
    this->_header.setArcount(static_cast<std::uint16_t>(this->_additional.size()));
}

void response::setQuestion(q_type &&value)
{
    this->_question = std::move(value);
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void response::setAnswer(rr_type &&value)
{
    this->_answer = std::move(value);
    this->_header.setAncount(static_cast<std::uint16_t>(this->_answer.size()));
}

void response::setAuthority(rr_type &&value)
{
    this->_authority = std::move(value);
    this->_header.setNscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void response::setAdditional(rr_type &&value)
{
    this->_additional = std::move(value);
    this->_header.setArcount(static_cast<std::uint16_t>(this->_additional.size()));
}

// rotate
void response::rotate()
{
    if (this->_answer.size() <= 1)
        return;

    auto rr = this->_answer.front();
    this->_answer.erase(this->_answer.begin());
    this->_answer.emplace_back(rr);
}

// codec
std::vector<std::uint8_t> response::encode() const
{
    chen::dns::encoder encoder;
    this->encode(encoder);
    return encoder.data();
}

void response::decode(chen::dns::codec::iterator beg, chen::dns::codec::iterator end)
{
    chen::dns::decoder decoder(beg, end);
    this->decode(decoder);
}

void response::encode(chen::dns::encoder &encoder) const
{
    // header
    message::encode(encoder);

    // question
    for (auto &val : this->_question)
        val.encode(encoder);

    // answer
    for (auto &val : this->_answer)
        val->encode(encoder);

    // authority
    for (auto &val : this->_authority)
        val->encode(encoder);

    // additional
    for (auto &val : this->_additional)
        val->encode(encoder);
}

void response::decode(chen::dns::decoder &decoder)
{
    // header
    message::decode(decoder);

    // question
    this->_question.clear();

    for (std::uint16_t i = 0, len = this->_header.qdcount(); i < len; ++i)
    {
        chen::dns::question q;
        q.decode(decoder);

        this->_question.emplace_back(q);
    }

    // answer
    this->_answer.clear();

    for (std::uint16_t i = 0, len = this->_header.ancount(); i < len; ++i)
    {
        this->_answer.emplace_back(chen::dns::RR::create(decoder));
    }

    // authority
    this->_authority.clear();

    for (std::uint16_t i = 0, len = this->_header.nscount(); i < len; ++i)
    {
        this->_authority.emplace_back(chen::dns::RR::create(decoder));
    }

    // additional
    this->_additional.clear();

    for (std::uint16_t i = 0, len = this->_header.arcount(); i < len; ++i)
    {
        this->_additional.emplace_back(chen::dns::RR::create(decoder));
    }
}