/**
 * Created by Jian Chen
 * @since  2015.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "udp_server.h"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server
server::server(recv_type callback)
: _func(callback)
{

}

void server::run(const std::string &addr, std::uint16_t port)
{
    // bind
    this->bind(addr, port);

    // listen
    while (true)
    {
        packet pkt;
        pkt.size = this->recv(pkt.data, sizeof(pkt.data), pkt.addr, pkt.port);

        this->_pool.async(std::bind(this->_func, pkt));
    }
}

std::size_t server::send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port)
{
    return socket::send(data, size, addr, port);
}