/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "tcp_client.hpp"

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// client
client::client()
{

}

// close
void client::close()
{
    socket::close();
    this->_connected = false;
}

// remote
std::string client::remoteHost() const
{
    return this->_remote_host;
}

std::string client::remoteAddr() const
{
    return this->_remote_addr;
}

std::uint16_t client::remotePort() const
{
    return this->_remote_port;
}

// local
std::string client::localAddr() const
{
    return this->_local_addr;
}

std::uint16_t client::localPort() const
{
    return this->_local_port;
}

// property
bool client::isConnected() const
{
    return this->_connected;
}