/**
 * Created by Jian Chen
 * @since  2015.12.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "dns_server.h"
#include "dns_codec.h"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// server
server::server()
{
    this->_udp.attach(std::bind(&server::onPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

// start
void server::bind(const std::string &addr, std::uint16_t port)
{
    this->_udp.bind(addr, port);
}

void server::start()
{
    this->_udp.start();
}

void server::stop()
{
    this->_udp.stop();
}

// callback
void server::attach(const callback_type &callback)
{
    this->_callback = callback;
}

void server::detach()
{
    this->_callback = nullptr;
}

void server::notify(const chen::dns::request &request)
{
    if (this->_callback)
        this->_callback(std::move(request));
}

// address
std::string server::addr() const
{
    return this->_udp.addr();
}

std::uint16_t server::port() const
{
    return this->_udp.port();
}

// handler
void server::onPacket(const std::vector<std::uint8_t> &data,
                      const std::string &addr,
                      std::uint16_t port)
{
    request q;

    q.setAddr(addr);
    q.setPort(port);
    codec::unpack(q, data.data(), data.size());

    this->notify(q);
}