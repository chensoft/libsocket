/**
 * Created by Jian Chen
 * @since  2015.12.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "dns_server.hpp"
#include "dns_codec.hpp"
#include "dns_packet.hpp"

using namespace chen;
using namespace chen::dns;

// -----------------------------------------------------------------------------
// server
server::server()
{
    this->_udp.attach(std::bind(&server::onPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

// start
void server::start()
{
    this->_udp.start();
}

void server::start(const std::string &addr, std::uint16_t port)
{
    this->_udp.start(addr, port);
}

void server::stop()
{
    this->_udp.stop();
}

// callback
void server::attach(callback_type callback)
{
    this->_callback = callback;
}

void server::detach()
{
    this->_callback = nullptr;
}

void server::notify(chen::dns::request request)
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

// bind
void server::bind(const std::string &addr, std::uint16_t port)
{
    this->_udp.bind(addr, port);
}

// handler
void server::onPacket(std::vector<std::uint8_t> data,
                      std::string addr,
                      std::uint16_t port)
{
    request q;
    decoder d;

    q.setAddr(addr);
    q.setPort(port);

    d.assign(data);
    d.unpack(q);

    this->notify(std::move(q));
}