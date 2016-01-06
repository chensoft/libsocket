/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
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
// request
request::request()
{
    // random id
    this->_header.setId();

    // set query
    this->_header.setQr(QR::Query);

    // set rd
    this->setRecursionDesired(true);
}

// get
const chen::dns::header& request::header() const
{
    return this->_header;
}

const chen::dns::question& request::question() const
{
    return this->_question;
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

// set
void request::setHeader(const chen::dns::header &value)
{
    this->_header = value;
}

void request::setQuestion(const chen::dns::question &value)
{
    this->_question = value;
}

// address
void request::setAddr(const std::string &value)
{
    this->_addr = value;
}

void request::setPort(std::uint16_t value)
{
    this->_port = value;
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
    if (!codec::isFqdn(qname))
        throw error_fqdn("request query name is not fqdn");

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
void request::setRecursionDesired(bool value)
{
    this->_header.setRecursionDesired(value);
}


// -----------------------------------------------------------------------------
// response

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