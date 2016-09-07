/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

// -----------------------------------------------------------------------------
// client
chen::tcp::client::client(ip::address::Type family) : basic(family, SOCK_STREAM)
{
}

// connection
void chen::tcp::client::connect(const endpoint &ep)
{
    if (!this->isDisconnect())
        this->disconnect();

    this->notifyConnecting(ep);

    auto status = basic::connect(ep);

    // if the socket is in non-blocking mode, then pending method will be true
    // if the socket is in blocking mode, we check error and notify to user
    if (!status || !status.pending())
        this->notifyConnected(ep, status);
}

void chen::tcp::client::connect(const ip::address &addr, std::uint16_t port)
{
    this->connect(endpoint(addr, port));
}

void chen::tcp::client::disconnect()
{
    this->close();
    this->_state = State::Disconnect;
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

bool chen::tcp::client::isDisconnect() const
{
    return this->_state == State::Disconnect;
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

// notify
void chen::tcp::client::notifyConnecting(endpoint ep)
{
    this->_state = State::Connecting;

    event::connecting ev;
    ev.ep = std::move(ep);

    if (this->_cb)
        this->_cb(*this, &ev);
}

void chen::tcp::client::notifyConnected(endpoint ep, chen::status err)
{
    this->_state = err ? State::Connected : State::Disconnect;

    event::connected ev;
    ev.ep  = std::move(ep);
    ev.err = err;

    if (this->_cb)
        this->_cb(*this, &ev);
}

void chen::tcp::client::notifyDisconnect(chen::status err)
{

}

void chen::tcp::client::notifyRead(std::vector<std::uint8_t> data)
{

}

void chen::tcp::client::notifyWrite(std::size_t size)
{

}

// event
void chen::tcp::client::onEvent(chen::notifier &n, chen::notifier::Event ev)
{
    // todo
}