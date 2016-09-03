/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

// -----------------------------------------------------------------------------
// client

// state
chen::tcp::client::State chen::tcp::client::state() const
{
    return this->_state;
}

bool chen::tcp::client::isConnected() const
{
    return this->_state == State::Connected;
}

bool chen::tcp::client::isConnecting() const
{
    return this->_state == State::Connecting;
}

// info
std::string chen::tcp::client::host() const
{
    return this->_host;
}

std::uint16_t chen::tcp::client::port() const
{
    return this->_port;
}