/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_packet.hpp>
#include <socket/dns/dns_record.hpp>
#include <algorithm>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// message

// property
message::header_type& message::header()
{
    return this->_header;
}

const message::header_type& message::header() const
{
    return this->_header;
}

const std::vector<message::question_type>& message::question() const
{
    return this->_question;
}

const std::vector<message::record_type>& message::answer() const
{
    return this->_answer;
}

const std::vector<message::record_type>& message::authority() const
{
    return this->_authority;
}

const std::vector<message::record_type>& message::additional() const
{
    return this->_additional;
}

void message::addQuestion(question_type value)
{
    this->_question.emplace_back(std::move(value));
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void message::addAnswer(record_type value)
{
    this->_answer.emplace_back(std::move(value));
    this->_header.setAncount(static_cast<std::uint16_t>(this->_answer.size()));
}

void message::addAuthority(record_type value)
{
    this->_authority.emplace_back(std::move(value));
    this->_header.setNscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void message::addAdditional(record_type value)
{
    this->_additional.emplace_back(std::move(value));
    this->_header.setArcount(static_cast<std::uint16_t>(this->_additional.size()));
}

void message::setQuestion(question_type value)
{
    this->_question.clear();
    this->_question.emplace_back(std::move(value));
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void message::setAnswer(record_type value)
{
    this->_answer.clear();
    this->_answer.emplace_back(std::move(value));
    this->_header.setAncount(static_cast<std::uint16_t>(this->_answer.size()));
}

void message::setAuthority(record_type value)
{
    this->_authority.clear();
    this->_authority.emplace_back(std::move(value));
    this->_header.setNscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void message::setAdditional(record_type value)
{
    this->_additional.clear();
    this->_additional.emplace_back(std::move(value));
    this->_header.setArcount(static_cast<std::uint16_t>(this->_additional.size()));
}

void message::setQuestion(std::vector<question_type> value)
{
    this->_question = std::move(value);
    this->_header.setQdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void message::setAnswer(std::vector<record_type> value)
{
    this->_answer = std::move(value);
    this->_header.setAncount(static_cast<std::uint16_t>(this->_answer.size()));
}

void message::setAuthority(std::vector<record_type> value)
{
    this->_authority = std::move(value);
    this->_header.setNscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void message::setAdditional(std::vector<record_type> value)
{
    this->_additional = std::move(value);
    this->_header.setArcount(static_cast<std::uint16_t>(this->_additional.size()));
}

// edns
std::shared_ptr<OPT> message::edns0() const
{
    for (auto &rr : this->_additional)
    {
        if (rr->rrtype == RRType::OPT)
        {
            // check edns version
            auto ret = std::dynamic_pointer_cast<OPT>(rr->clone());
            if (ret->version() == 0)
                return ret;
        }
    }

    return nullptr;
}

// codec
void message::encode(encoder &encoder) const
{
    // header
    this->_header.encode(encoder);

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

void message::decode(decoder &decoder)
{
    // header
    this->_header.decode(decoder);

    // question
    this->_question.clear();

    for (std::uint16_t i = 0, len = this->_header.qdcount(); i < len; ++i)
    {
        dns::question q;
        q.decode(decoder);

        this->_question.emplace_back(std::move(q));
    }

    // answer
    this->_answer.clear();

    for (std::uint16_t i = 0, len = this->_header.ancount(); i < len; ++i)
    {
        auto record = RR::create(decoder);

        if (record)
            this->_answer.emplace_back(std::move(record));
        else
            throw error_codec("dns: decode answer error, unknown record detect");
    }

    // authority
    this->_authority.clear();

    for (std::uint16_t i = 0, len = this->_header.nscount(); i < len; ++i)
    {
        auto record = RR::create(decoder);

        if (record)
            this->_authority.emplace_back(std::move(record));
        else
            throw error_codec("dns: decode authority error, unknown record detect");
    }

    // additional
    this->_additional.clear();

    for (std::uint16_t i = 0, len = this->_header.arcount(); i < len; ++i)
    {
        auto record = RR::create(decoder);

        if (record)
            this->_additional.emplace_back(std::move(record));
        else
            throw error_codec("dns: decode additional error, unknown record detect");
    }
}


// -----------------------------------------------------------------------------
// request
request::request(const std::string &qname, RRType qtype)
{
    this->setQuery(qname, qtype);
}

// query
const request::question_type& request::query() const
{
    if (this->_question.empty())
        throw error("dns: request question is empty");

    return this->_question[0];
}

request::question_type& request::query()
{
    if (this->_question.empty())
        throw error("dns: request question is empty");

    return this->_question[0];
}

void request::setQuery(const std::string &qname, RRType qtype)
{
    // check empty
    if (qname.empty())
        throw error_codec("dns: request query name is empty");

    // check fqdn
    if (!codec::isFqdn(qname))
        throw error_fqdn("dns: request query name is not fqdn");

    // set id
    this->_header.setId(header::random());

    // set qr
    this->_header.setQr(QR::Query);

    // set opcode
    this->_header.setOpcode(OPCODE::Query);

    // set recursion desired
    this->_header.setRecursionDesired(true);

    // set question
    this->setQuestion(question_type());

    question_type &question = this->_question[0];
    question.setQname(qname);
    question.setQtype(qtype);
    question.setQclass(RRClass::IN);
}

// client
std::string request::addr(bool subnet) const
{
    if (subnet)
    {
        auto option = this->option<edns0::Subnet>();
        if (option)
            return option->address.str();
    }

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
    encoder encoder;
    this->encode(encoder);
    return encoder.data();
}

void request::decode(codec::iterator beg, codec::iterator end,
                     const std::string &addr, std::uint16_t port)
{
    decoder decoder(beg, end);
    this->decode(decoder);

    this->setAddr(addr);
    this->setPort(port);
}


// -----------------------------------------------------------------------------
// response
response::response(bool authoritative)
{
    this->_header.setQr(QR::Response);
    this->_header.setOpcode(OPCODE::Query);
    this->_header.setAuthoritative(authoritative);
    this->_header.setRcode(RCODE::NoError);
}

response::response(bool authoritative, const request &request) : response(authoritative)
{
    this->setQuestion(request);
}

// question
void response::setQuestion(const request &request)
{
    this->_header.setId(request.header().id());
    this->_header.setRecursionDesired(request.header().recursionDesired());

    this->setQuestion(request.question());

    // include the OPT record
    // rfc6891, section 6.1.1
    auto edns0 = request.edns0();
    if (edns0)
        this->setAdditional(std::move(edns0));
}

// rotate
void response::rotate()
{
    if (this->_answer.size() <= 1)
        return;

    auto rr = this->_answer.front();
    this->_answer.erase(this->_answer.begin());
    this->_answer.emplace_back(std::move(rr));
}

// random
void response::random()
{
    if (this->_answer.size() <= 1)
        return;

    std::random_shuffle(this->_answer.begin(), this->_answer.end());
}

// codec
std::vector<std::uint8_t> response::encode() const
{
    encoder encoder;
    this->encode(encoder);
    return encoder.data();
}

void response::decode(codec::iterator beg, codec::iterator end)
{
    decoder decoder(beg, end);
    this->decode(decoder);
}