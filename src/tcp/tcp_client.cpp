/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>

// -----------------------------------------------------------------------------
// client
chen::tcp::client::client(socket_t fd) : basic(fd)
{
}

chen::tcp::client::client(ip::address::Type family) : basic(family)
{
}

// connection
void chen::tcp::client::connect(const net::endpoint &ep)
{

}

void chen::tcp::client::connect(const ip::address &addr, std::uint16_t port)
{

}

void chen::tcp::client::reconnect()
{

}

void chen::tcp::client::disconnect()
{

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

std::string chen::tcp::client::host() const
{
    return this->_host;
}

std::uint16_t chen::tcp::client::port() const
{
    return this->_port;
}

// event
void chen::tcp::client::attach(tcp::event::Type type, std::function<void (chen::tcp::client &c, chen::tcp::event &e)> callback)
{
    this->_event[static_cast<int>(type)] = callback;
}

void chen::tcp::client::detach(tcp::event::Type type)
{
    this->_event.erase(static_cast<int>(type));
}

void chen::tcp::client::notify(tcp::connecting_event ev)
{

}

void chen::tcp::client::notify(tcp::connected_event ev)
{

}

void chen::tcp::client::notify(tcp::disconnect_event ev)
{

}

void chen::tcp::client::notify(tcp::send_event ev)
{

}

void chen::tcp::client::notify(tcp::recv_event ev)
{

}

void chen::tcp::client::onEventSend(std::size_t size, std::error_code error)
{

}

void chen::tcp::client::onEventRecv(std::vector<std::uint8_t> data, std::error_code error)
{

}

void chen::tcp::client::onEventEOF()
{

}

// handy
chen::tcp::client chen::tcp::client::v4()
{
    return client(ip::address::Type::IPv4);
}

chen::tcp::client chen::tcp::client::v6()
{
    return client(ip::address::Type::IPv6);
}




//// connection
//void chen::tcp::client::connect(const endpoint &ep)
//{
//    if (!this->isDisconnect())
//        this->disconnect();
//
//    this->notifyConnecting(ep);
//
//    auto status = basic::connect(ep);
//
//    // if the socket is in non-blocking mode, then pending method will be true
//    // if the socket is in blocking mode, we check error and notify to user
//    if (!status || (status != std::errc::resource_unavailable_try_again))
//        this->notifyConnected(ep, status);
//}
//
//void chen::tcp::client::connect(const ip::address &addr, std::uint16_t port)
//{
//    this->connect(endpoint(addr, port));
//}
//
//void chen::tcp::client::disconnect()
//{
//    this->close();
//    this->_state = State::Disconnect;
//}
//
//// notify
//void chen::tcp::client::notifyConnecting(endpoint ep)
//{
//    this->_state = State::Connecting;
//
//    event::connecting ev;
//    ev.ep = std::move(ep);
//
//    if (this->_cb)
//        this->_cb(*this, &ev);
//}
//
//void chen::tcp::client::notifyConnected(endpoint ep, std::error_code err)
//{
//    this->_state = err ? State::Connected : State::Disconnect;
//
//    event::connected ev;
//    ev.ep  = std::move(ep);
//    ev.err = err;
//
//    if (this->_cb)
//        this->_cb(*this, &ev);
//}