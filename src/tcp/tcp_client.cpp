/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// client
client::client()
{

}

// connect
bool client::reconnect(float timeout)
{
    return this->connect(this->_recent_addr, this->_recent_port, timeout);
}

void client::disconnect()
{
    this->shutdown();
    this->close();
}

// property
std::string client::recentAddr() const
{
    return this->_recent_addr;
}

std::uint16_t client::recentPort() const
{
    return this->_recent_port;
}

bool client::isConnected() const
{
    return this->_connected;
}

// close
void client::close()
{
    socket::close();
    this->_connected = false;
}