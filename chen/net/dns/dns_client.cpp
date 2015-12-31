/**
 * Created by Jian Chen
 * @since  2015.12.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_client.h"
#include "dns_error.h"
#include "dns_codec.h"
#include <array>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// client

// nameserver
void client::setNameserver(const std::string &addr, std::uint16_t port)
{
    this->_ns_addr = addr;
    this->_ns_port = port;
}

// resolve
chen::dns::response client::query(const std::string &qname,
                                  chen::dns::RRType qtype,
                                  chen::dns::RRClass qclass)
{
    // check nameserver
    if (this->_ns_addr.empty() || !this->_ns_port)
        throw error_nameserver("client query nameserver ip or port is empty");

    // build request
    request q;
    q.setQuery(qname, qtype, qclass);
    q.setRecursionDesired(true);

    // build binary
    encoder e;
    e.pack(q);

    // send request
    auto b = e.binary();
    this->_udp.send(b.data(), b.size(), this->_ns_addr, this->_ns_port);

    // recv response
    std::array<std::uint8_t, 65507> packet;
    std::string addr;
    std::uint16_t port = 0;
    std::size_t size = packet.size();

    this->_udp.recv(packet.data(), size, addr, port);

    // build response
    response r;
    decoder d;

    d.assign(std::vector<std::uint8_t>(packet.begin(), packet.begin() + size));
    d.unpack(r);

    return std::move(r);
}