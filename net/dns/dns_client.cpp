/**
 * Created by Jian Chen
 * @since  2015.10.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_client.h"
#include "dns_error.h"
#include "dns_tool.h"
#include "tool/log.h"
#include "net/udp/udp_packet.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// client
client::client()
{

}

// server
void client::setNameserver(const std::vector<chen::ip::address_v4> &list)
{
    this->_server = list;
}

// resolve
chen::dns::response client::resolve(const std::string &qname,
                                    chen::dns::RRType qtype,
                                    chen::dns::RRClass qclass)
{
    // pick nameserver
    ip::address_v4 v4;

    if (this->_server.empty())
    {
        auto list = chen::dns::tool::nameservers();
        if (list.empty())
            throw error_nameserver("client resolve nameserver is empty");

        std::srand(static_cast<unsigned>(std::time(nullptr)));
        v4 = list[std::rand() % list.size()];
    }
    else
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        v4 = this->_server[std::rand() % this->_server.size()];
    }

    // build query
    request q;
    q.setQuery(qname, qtype, qclass);
    q.setRecursionDesired();

    // send query
    auto binary = q.binary();

    std::string addr(v4.str());
    std::uint16_t port = 53;

    this->_udp.send(binary.data(), binary.size(), addr, port);

    // recv response
    udp::packet pkt;
    pkt.data.resize(65507);
    pkt.size = this->_udp.recv(pkt.data.data(), pkt.data.size(), pkt.addr, pkt.port);

    response r;
    r.assign(pkt.data.data(), pkt.size);

    return r;
}