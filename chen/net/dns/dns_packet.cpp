/**
 * Created by Jian Chen
 * @since  2015.11.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_packet.h"
#include "dns_error.h"
#include "dns_codec.h"
#include <chen/tool/str.h>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// message


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

// field value
const chen::dns::header& request::header() const
{
    return this->_header;
}

const chen::dns::question& request::question() const
{
    return this->_question;
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


//// -----------------------------------------------------------------------------
//// response
//
//// header
//chen::dns::header& response::header()
//{
//    return this->_header;
//}
//
//// rrs
//response::q_type& response::question()
//{
//    return this->_question;
//}
//
//response::rr_type& response::answer()
//{
//    return this->_answer;
//}
//
//response::rr_type& response::authority()
//{
//    return this->_authority;
//}
//
//response::rr_type& response::additional()
//{
//    return this->_additional;
//}