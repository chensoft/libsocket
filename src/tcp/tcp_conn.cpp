/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_handler.hpp>
#include <socket/tcp/tcp_conn.hpp>

// -----------------------------------------------------------------------------
// conn
chen::tcp::conn::conn(bsd::socket &&s, std::unique_ptr<handler> &&h) : _handler(std::move(h))
{
    this->reset(std::move(s));
}

chen::tcp::conn::~conn()
{
    if (this->_socket)
    {
        this->_socket.close();
        this->disconnect();
    }
}

void chen::tcp::conn::disconnect()
{
    // we shutdown the socket here, server will
    // receive the ended event and destroy object
    if (this->_socket)
        this->_socket.shutdown();

    this->_connected = false;
}

// property
bool chen::tcp::conn::isConnected() const
{
    return this->_connected;
}

bool chen::tcp::conn::isDisconnect() const
{
    return !this->_connected;
}

chen::socket_t chen::tcp::conn::native() const
{
    return this->_socket.native();
}

// event
void chen::tcp::conn::onAccepted()
{
    this->_handler->onAccepted(*this);
}

void chen::tcp::conn::onReadable()
{

}

void chen::tcp::conn::onWritable()
{

}

void chen::tcp::conn::onEnded()
{
    this->_handler->onDisconnect(*this);
}