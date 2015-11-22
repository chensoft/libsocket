/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "udp_server.h"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server
server::server()
{

}

void server::start()
{
    // define maximum udp buffer
    // 65535 − 8 byte UDP header − 20 byte IP header = 65507
    // see https://en.wikipedia.org/wiki/User_Datagram_Protocol
    std::size_t length = 65507;
    std::unique_ptr<std::uint8_t> pointer(new std::uint8_t[length]);
    std::uint8_t *buffer = pointer.get();

    while (true)
    {
        // receive data from remote
        std::string addr;
        std::uint16_t port = 0;
        std::size_t size = length;

        this->recv(buffer, size, addr, port);

        // post result to callback
        this->notify(std::vector<std::uint8_t>(buffer, buffer + size), addr, port);
    }
}

void server::attach(const callback_type &callback)
{
    this->_callback = callback;
}

void server::detach()
{
    this->_callback = nullptr;
}

void server::notify(std::vector<std::uint8_t> data, std::string addr, std::uint16_t port)
{
    if (this->_callback)
        this->_callback(std::move(data), std::move(addr), port);
}

std::string server::addr() const
{
    return this->_addr;
}

std::uint16_t server::port() const
{
    return this->_port;
}