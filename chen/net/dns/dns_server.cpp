/**
 * Created by Jian Chen
 * @since  2015.11.01
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_server.h"
#include "dns_error.h"
#include "dns_tool.h"
#include <chen/tool/log.h>

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// server
server::server()
: _udp(std::bind(&server::callback, this, std::placeholders::_1, std::placeholders::_2))
{

}

// insert & remove
void server::insert(const std::string &zone, handler_type handler)
{
    if (zone.empty())
        throw error_empty("server insert zone is empty");

    if (!handler)
        throw error_empty("server insert handler is empty");

    if (!tool::isFqdn(zone))
        throw error_fqdn("server insert zone is not fqdn");

    this->_cache[zone] = handler;
}

void server::remove(const std::string &zone)
{
    auto it = this->_cache.find(zone);
    if (it != this->_cache.end())
        this->_cache.erase(it);
}

// run
void server::run(const std::string &addr, std::uint16_t port)
{
    this->_udp.run(addr, port);
}

// callback
void server::callback(chen::udp::socket *so, const chen::udp::packet &pkt) {
    // todo
    PILogE("haha: %s", tool::format(pkt.data).c_str());
}