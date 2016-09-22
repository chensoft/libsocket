/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

// -----------------------------------------------------------------------------
// client
chen::tcp::client::client(ip::address::Type family, net::proactor &proactor) : basic(family), _proactor(proactor)
{
    this->nonblocking(true);
}

// connection
void chen::tcp::client::connect(const net::endpoint &ep)
{
    if (!this->isDisconnect())
        this->disconnect();

    this->notify(tcp::connecting_event(ep));

    // connect to remote host, wait for the send event
    this->_proactor.send(this, {});
    this->_handle.connect(ep);
}

void chen::tcp::client::connect(const ip::address &addr, std::uint16_t port)
{
    this->connect(net::endpoint(addr, port));
}

void chen::tcp::client::reconnect()
{
    // todo
}

void chen::tcp::client::disconnect()
{
    // todo reset handle
    this->_handle.close();
    this->_state = State::Disconnect;
}

// property
bool chen::tcp::client::isConnecting() const
{
    return this->_state == State::Connecting;
}

bool chen::tcp::client::isConnected() const
{
    return this->_state == State::Connected;
}

bool chen::tcp::client::isDisconnect() const
{
    return this->_state == State::Disconnect;
}

chen::net::endpoint chen::tcp::client::remote() const
{
    return this->_remote;
}

// event
void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::connecting_event &e)> callback)
{
    this->_cb_connecting = callback;
}

void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::connected_event &e)> callback)
{
    this->_cb_connected = callback;
}

void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::disconnect_event &e)> callback)
{
    this->_cb_disconnect = callback;
}

void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::send_event &e)> callback)
{
    this->_cb_send = callback;
}

void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::recv_event &e)> callback)
{
    this->_cb_recv = callback;
}

void chen::tcp::client::detach(Event type)
{
    switch (type)
    {
        case Event::Connecting:
            this->_cb_connecting = nullptr;
            break;

        case Event::Connected:
            this->_cb_connected = nullptr;
            break;

        case Event::Disconnect:
            this->_cb_disconnect = nullptr;
            break;

        case Event::Send:
            this->_cb_send = nullptr;
            break;

        case Event::Recv:
            this->_cb_recv = nullptr;
            break;
    }
}

void chen::tcp::client::notify(tcp::connecting_event &&ev)
{
    this->_state  = State::Connecting;
    this->_remote = ev.ep;

    if (this->_cb_connecting)
        this->_cb_connecting(*this, ev);
}

void chen::tcp::client::notify(tcp::connected_event &&ev)
{
    this->_state = !ev.err ? State::Connected : State::Disconnect;

    if (this->_cb_connected)
        this->_cb_connected(*this, ev);
}

void chen::tcp::client::notify(tcp::disconnect_event &&ev)
{
    this->_state = State::Disconnect;

    if (this->_cb_disconnect)
        this->_cb_disconnect(*this, ev);
}

void chen::tcp::client::notify(tcp::send_event &&ev)
{
    if (this->_cb_send)
        this->_cb_send(*this, ev);
}

void chen::tcp::client::notify(tcp::recv_event &&ev)
{
    if (this->_cb_recv)
        this->_cb_recv(*this, ev);
}

void chen::tcp::client::onEventSend(std::size_t size, std::error_code error)
{
    if (this->isConnecting())
    {
        // socket can be written and error is nil means connected successful
        if (error)
            this->disconnect();

        this->notify(connected_event(this->_remote, error));
    }
    else if (this->isConnected())
    {
        // disconnect if error occur, otherwise notify the send callback
        if (!error)
        {
            this->notify(send_event(size));
        }
        else
        {
            this->disconnect();
            this->notify(disconnect_event(error));
        }
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received send event");
    }
}

void chen::tcp::client::onEventRecv(std::vector<std::uint8_t> data, std::error_code error)
{
    if (this->isConnected())
    {
        // disconnect if error occur, otherwise notify the recv callback
        if (!error)
        {
            this->notify(recv_event(std::move(data)));
        }
        else
        {
            this->disconnect();
            this->notify(disconnect_event(error));
        }
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received recv event");
    }
}

void chen::tcp::client::onEventEOF()
{
    if (this->isConnecting())
    {
        // receive eof when connecting usually means connection refused
        this->disconnect();
        this->notify(connected_event(this->_remote, std::make_error_code(std::errc::connection_refused)));
    }
    else if (this->isConnected())
    {
        // connection broken
        this->disconnect();
        this->notify(disconnect_event({}));
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received eof event");
    }
}

// handy
chen::tcp::client chen::tcp::client::v4(net::proactor &proactor)
{
    return client(ip::address::Type::IPv4, proactor);
}

chen::tcp::client chen::tcp::client::v6(net::proactor &proactor)
{
    return client(ip::address::Type::IPv6, proactor);
}