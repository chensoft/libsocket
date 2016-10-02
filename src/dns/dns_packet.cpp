/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/dns/dns_packet.hpp>
#include <socket/dns/dns_record.hpp>
#include <algorithm>

// -----------------------------------------------------------------------------
// message

// property
chen::dns::message::header_type& chen::dns::message::header()
{
    return this->_header;
}

const chen::dns::message::header_type& chen::dns::message::header() const
{
    return this->_header;
}

const std::vector<chen::dns::message::question_type>& chen::dns::message::question() const
{
    return this->_question;
}

const std::vector<chen::dns::message::record_type>& chen::dns::message::answer() const
{
    return this->_answer;
}

const std::vector<chen::dns::message::record_type>& chen::dns::message::authority() const
{
    return this->_authority;
}

const std::vector<chen::dns::message::record_type>& chen::dns::message::additional() const
{
    return this->_additional;
}

void chen::dns::message::addQuestion(question_type value)
{
    this->_question.emplace_back(std::move(value));
    this->_header.qdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void chen::dns::message::addAnswer(record_type value)
{
    this->_answer.emplace_back(std::move(value));
    this->_header.ancount(static_cast<std::uint16_t>(this->_answer.size()));
}

void chen::dns::message::addAuthority(record_type value)
{
    this->_authority.emplace_back(std::move(value));
    this->_header.nscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void chen::dns::message::addAdditional(record_type value)
{
    this->_additional.emplace_back(std::move(value));
    this->_header.arcount(static_cast<std::uint16_t>(this->_additional.size()));
}

void chen::dns::message::setQuestion(question_type value)
{
    this->_question.clear();
    this->_question.emplace_back(std::move(value));
    this->_header.qdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void chen::dns::message::setAnswer(record_type value)
{
    this->_answer.clear();
    this->_answer.emplace_back(std::move(value));
    this->_header.ancount(static_cast<std::uint16_t>(this->_answer.size()));
}

void chen::dns::message::setAuthority(record_type value)
{
    this->_authority.clear();
    this->_authority.emplace_back(std::move(value));
    this->_header.nscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void chen::dns::message::setAdditional(record_type value)
{
    this->_additional.clear();
    this->_additional.emplace_back(std::move(value));
    this->_header.arcount(static_cast<std::uint16_t>(this->_additional.size()));
}

void chen::dns::message::setQuestion(std::vector<question_type> value)
{
    this->_question = std::move(value);
    this->_header.qdcount(static_cast<std::uint16_t>(this->_question.size()));
}

void chen::dns::message::setAnswer(std::vector<record_type> value)
{
    this->_answer = std::move(value);
    this->_header.ancount(static_cast<std::uint16_t>(this->_answer.size()));
}

void chen::dns::message::setAuthority(std::vector<record_type> value)
{
    this->_authority = std::move(value);
    this->_header.nscount(static_cast<std::uint16_t>(this->_authority.size()));
}

void chen::dns::message::setAdditional(std::vector<record_type> value)
{
    this->_additional = std::move(value);
    this->_header.arcount(static_cast<std::uint16_t>(this->_additional.size()));
}

// edns
std::shared_ptr<chen::dns::OPT> chen::dns::message::edns0() const
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
void chen::dns::message::encode(dns::encoder &encoder) const
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

void chen::dns::message::decode(dns::decoder &decoder)
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
            throw std::runtime_error("dns: decode answer error, unknown record detect");
    }

    // authority
    this->_authority.clear();

    for (std::uint16_t i = 0, len = this->_header.nscount(); i < len; ++i)
    {
        auto record = RR::create(decoder);

        if (record)
            this->_authority.emplace_back(std::move(record));
        else
            throw std::runtime_error("dns: decode authority error, unknown record detect");
    }

    // additional
    this->_additional.clear();

    for (std::uint16_t i = 0, len = this->_header.arcount(); i < len; ++i)
    {
        auto record = RR::create(decoder);

        if (record)
            this->_additional.emplace_back(std::move(record));
        else
            throw std::runtime_error("dns: decode additional error, unknown record detect");
    }
}


// -----------------------------------------------------------------------------
// request
chen::dns::request::request(const std::string &qname, RRType qtype)
{
    this->query(qname, qtype);
}

// query
const chen::dns::request::question_type& chen::dns::request::query() const
{
    if (this->_question.empty())
        throw std::runtime_error("dns: request question is empty");

    return this->_question[0];
}

chen::dns::request::question_type& chen::dns::request::query()
{
    if (this->_question.empty())
        throw std::runtime_error("dns: request question is empty");

    return this->_question[0];
}

void chen::dns::request::query(const std::string &qname, RRType qtype)
{
    // check empty
    if (qname.empty())
        throw std::runtime_error("dns: request query name is empty");

    // check fqdn
    if (!codec::isFqdn(qname))
        throw std::runtime_error("dns: request query name is not fqdn");

    // set id
    this->_header.id(header::random());

    // set qr
    this->_header.qr(QR::Query);

    // set opcode
    this->_header.opcode(OPCODE::Query);

    // set recursion desired
    this->_header.recursionDesired(true);

    // set question
    this->setQuestion(question_type());

    question_type &question = this->_question[0];
    question.qname(qname);
    question.qtype(qtype);
    question.qclass(RRClass::IN);
}

// client
std::string chen::dns::request::addr() const
{
    return this->_addr;
}

std::string chen::dns::request::real() const
{
    auto option = this->option<edns0::Subnet>();
    return option ? option->address.str() : this->_addr;
}

std::uint16_t chen::dns::request::port() const
{
    return this->_port;
}

void chen::dns::request::addr(const std::string &addr)
{
    this->_addr = addr;
}

void chen::dns::request::port(std::uint16_t port)
{
    this->_port = port;
}

// codec
std::vector<std::uint8_t> chen::dns::request::encode() const
{
    encoder encoder;
    this->encode(encoder);
    return encoder.data();
}

void chen::dns::request::decode(codec::iterator beg, codec::iterator end,
                                const std::string &addr, std::uint16_t port)
{
    decoder decoder(beg, end);
    this->decode(decoder);

    this->addr(addr);
    this->port(port);
}


// -----------------------------------------------------------------------------
// response
chen::dns::response::response(bool authoritative)
{
    this->_header.qr(QR::Response);
    this->_header.opcode(OPCODE::Query);
    this->_header.authoritative(authoritative);
    this->_header.rcode(RCODE::NoError);
}

chen::dns::response::response(bool authoritative, const dns::request &request) : response(authoritative)
{
    this->setQuestion(request);
}

// question
void chen::dns::response::setQuestion(const dns::request &request)
{
    this->_header.id(request.header().id());
    this->_header.recursionDesired(request.header().recursionDesired());

    this->setQuestion(request.question());

    // include the OPT record
    // rfc6891, section 6.1.1
    auto edns0 = request.edns0();
    if (edns0)
        this->setAdditional(std::move(edns0));
}

// rotate
void chen::dns::response::rotate()
{
    if (this->_answer.size() <= 1)
        return;

    auto rr = this->_answer.front();
    this->_answer.erase(this->_answer.begin());
    this->_answer.emplace_back(std::move(rr));
}

// random
void chen::dns::response::random()
{
    if (this->_answer.size() <= 1)
        return;

    std::random_shuffle(this->_answer.begin(), this->_answer.end());
}

// codec
std::vector<std::uint8_t> chen::dns::response::encode() const
{
    encoder encoder;
    this->encode(encoder);
    return encoder.data();
}

void chen::dns::response::decode(codec::iterator beg, codec::iterator end)
{
    decoder decoder(beg, end);
    this->decode(decoder);
}