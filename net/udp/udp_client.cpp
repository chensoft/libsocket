/**
 * Created by Jian Chen
 * @since  2015.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "udp_client.h"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// client
client::client()
{
    this->build();
}

std::size_t client::send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port)
{
    return socket::send(data, size, addr, port);
}

std::size_t client::recv(std::uint8_t *data, std::size_t size, std::string &addr, std::uint16_t &port)
{
    return socket::recv(data, size, addr, port);
}