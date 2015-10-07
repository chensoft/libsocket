/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_client.h"
#include "dns_tool.h"
#include "tool/log.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// client
client::client()
{

}

// server
void client::setNameserver(const std::vector<std::string> &list)
{
    this->_server = list;
}

// resolve
chen::dns::response client::resolve(const std::string &qname,
                                    chen::dns::RRType qtype,
                                    chen::dns::RRClass qclass)
{
    question q;
    q.setQuery(qname, qtype, qclass);

    // todo delete
    PILogE("%s", tool::format(q.binary()).c_str());

    return response();
}