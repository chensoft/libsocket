/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

// -----------------------------------------------------------------------------
// client

// connection
void chen::tcp::client::connect(const endpoint &ep)
{
    this->connect(ep.addr(), ep.port());
}

void chen::tcp::client::connect(const ip::address &addr, std::uint16_t port)
{
    if (!this->isDisconnected())
        this->disconnect();

    this->reset(addr, SOCK_STREAM);

    // todo connect and notify, set connecting state
}

void chen::tcp::client::disconnect()
{
    this->close();

    // todo clear buffer and state
}

// event
void chen::tcp::client::attach(callback_type cb)
{
    this->_cb = cb;
}

void chen::tcp::client::detach()
{
    this->_cb = nullptr;
}

// state
chen::tcp::client::State chen::tcp::client::state() const
{
    return this->_state;
}

bool chen::tcp::client::isDisconnected() const
{
    return this->_state == State::Disconnected;
}

bool chen::tcp::client::isConnecting() const
{
    return this->_state == State::Connecting;
}

bool chen::tcp::client::isConnected() const
{
    return this->_state == State::Connected;
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

// event
void chen::tcp::client::onEvent(chen::notifier &n, chen::notifier::Event ev)
{
    // todo
}