/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

// -----------------------------------------------------------------------------
// client
chen::tcp::client::client(ip::address::Type family, net::notifier &notifier) : basic(family), _notifier(notifier)
{
    this->nonblocking(true);
}

chen::tcp::client::~client()
{
    this->_notifier.remove(this);
}

// connection
void chen::tcp::client::connect(const net::endpoint &ep)
{
    if (!this->isDisconnect())
        this->disconnect();

    this->notify(tcp::connecting_event(ep));

    // connect to remote host, wait for the write event
    this->_notifier.write(this, {});

    // todo must check methods' return code, if error then notify connected immediately
    this->_handle.connect(ep);
}

void chen::tcp::client::reconnect()
{
    // todo
}

void chen::tcp::client::disconnect()
{
    // todo many calls for disconnect is wrong, fd is reset to -1, can't do connect again
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

void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::read_event &e)> callback)
{
    this->_cb_read = callback;
}

void chen::tcp::client::attach(std::function<void (chen::tcp::client &c, chen::tcp::write_event &e)> callback)
{
    this->_cb_write = callback;
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

        case Event::Read:
            this->_cb_read = nullptr;
            break;

        case Event::Write:
            this->_cb_write = nullptr;
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

void chen::tcp::client::notify(tcp::read_event &&ev)
{
    if (this->_cb_read)
        this->_cb_read(*this, ev);
}

void chen::tcp::client::notify(tcp::write_event &&ev)
{
    if (this->_cb_write)
        this->_cb_write(*this, ev);
}

void chen::tcp::client::onRead(std::vector<std::uint8_t> data, net::endpoint ep, std::error_code error)
{
    if (this->isConnected())
    {
        // disconnect if error occur, otherwise notify the read callback
        if (!error)
        {
            this->notify(read_event(std::move(data)));
        }
        else
        {
            this->disconnect();
            this->notify(disconnect_event(error));
        }
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received read event");
    }
}

void chen::tcp::client::onWrite(std::size_t size, net::endpoint ep, std::error_code error)
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
        // disconnect if error occur, otherwise notify the write callback
        if (!error)
        {
            this->notify(write_event(size));
        }
        else
        {
            this->disconnect();
            this->notify(disconnect_event(error));
        }
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received write event");
    }
}

void chen::tcp::client::onEnd()
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