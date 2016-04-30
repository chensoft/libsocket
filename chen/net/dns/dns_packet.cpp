/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "dns_packet.hpp"
#include "dns_error.hpp"
#include "dns_codec.hpp"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// message
message::~message()
{

}


// -----------------------------------------------------------------------------
// query
query::query()
{
    // random id
    this->_header.setId();

    // set query
    this->_header.setQr(QR::Query);

    // set rd
    this->setRecursionDesired(true);
}

// get
const chen::dns::header& query::header() const
{
    return this->_header;
}

const chen::dns::question& query::question() const
{
    return this->_question;
}

// address
std::string query::addr() const
{
    return this->_addr;
}

std::uint16_t query::port() const
{
    return this->_port;
}

// set
void query::setHeader(const chen::dns::header &value)
{
    this->_header = value;
}

void query::setQuestion(const chen::dns::question &value)
{
    this->_question = value;
}

// address
void query::setAddr(const std::string &value)
{
    this->_addr = value;
}

void query::setPort(std::uint16_t value)
{
    this->_port = value;
}

// set query
void query::setQuery(const std::string &qname,
                       chen::dns::RRType qtype,
                       chen::dns::RRClass qclass)
{
    // check empty
    if (qname.empty())
        throw error_size("dns: request query name is empty");

    // check fqdn
    if (!codec::isFqdn(qname))
        throw error_fqdn("dns: request query name is not fqdn");

    // set opcode
    this->_header.setOpcode(chen::dns::OPCODE::Query);

    // set qdcount
    this->_header.setQdcount(1);

    // set query
    this->_question.setQname(qname);
    this->_question.setQtype(qtype);
    this->_question.setQclass(qclass);
}

// flag
void query::setRecursionDesired(bool value)
{
    this->_header.setRecursionDesired(value);
}


// -----------------------------------------------------------------------------
// response
response::response(const response &o)
{
    *this = o;
}

response& response::operator=(const response &o)
{
    if (this == &o)
        return *this;

    this->_header     = o._header;
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

    this->_header     = std::move(o._header);
    this->_question   = std::move(o._question);
    this->_answer     = std::move(o._answer);
    this->_authority  = std::move(o._authority);
    this->_additional = std::move(o._additional);

    return *this;
}

// get
const chen::dns::header& response::header() const
{
    return this->_header;
}

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
void response::setHeader(const chen::dns::header &value)
{
    this->_header = value;
}

void response::setQuestion(const q_type &value)
{
    this->_question = value;
}

void response::setAnswer(const rr_type &value)
{
    this->_answer = value;
}

void response::setAuthority(const rr_type &value)
{
    this->_authority = value;
}

void response::setAdditional(const rr_type &value)
{
    this->_additional = value;
}