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

// connect
void client::reconnect(float timeout)
{
    this->connect(this->_recent_addr, this->_recent_port, timeout);
}

void client::disconnect()
{
    this->shutdown();
    this->close();
}

// property
bool client::connected() const
{
    return this->_connected;
}

// close
void client::close()
{
    socket::close();
    this->_connected = false;
}