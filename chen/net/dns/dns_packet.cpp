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

    // check total length
    // caution: this limit isn't name's length, it's the bytes after encoded
    // example: www.chensoft.com. will encoded as [3, w, w, w, 8, c, h, e, n, s, o, f, t, 3, c, o, m, 0]
    // the encoded bytes can't exceed SIZE_LIMIT_NAME
    if (qname.size() + 1 > SIZE_LIMIT_NAME)
        throw error_size(str::format("request query name length must be %d octets or less", SIZE_LIMIT_NAME));

//    // check each label
//    std::string::size_type count = 0;
//
//    for (std::uint8_t i = 0, len = static_cast<std::uint8_t>(qname.size()); i < len; ++i)
//    {
//        char c = qname[i];
//
//        if (c == '.')
//        {
//            if (!count)
//                throw error_size("request query label is empty");
//
//            count = 0;
//        }
//        else
//        {
//            ++count;
//
//            if (count > 63)
//                throw error_size("request query label length must be 63 or less");
//        }
//    }
//
//    // set opcode
//    this->_header.setOpcode(chen::dns::OPCODE::Query);
//
//    // set qdcount
//    this->_header.setQdcount(1);
//
//    // set query
//    this->_question.setQname(qname);
//    this->_question.setQtype(qtype);
//    this->_question.setQclass(qclass);
}

// flag
void request::setRecursionDesired()
{
    this->_header.setRecursionDesired(true);
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