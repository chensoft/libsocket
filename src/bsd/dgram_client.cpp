/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/dgram_client.hpp>

// -----------------------------------------------------------------------------
// dgram_client
chen::dgram_client::dgram_client(runloop &loop, int family, int protocol) : _runloop(loop)
{
    this->_socket.reset(family, SOCK_DGRAM, protocol);
    this->_socket.nonblocking(true);

    this->_runloop.set(this->_socket.native(), runloop::OpcodeRW, runloop::FlagEdge,
                       std::bind(&dgram_client::onEvent, this, std::placeholders::_1));
}

chen::dgram_client::~dgram_client()
{
    if (this->_socket)
    {
        this->_runloop.del(this->_socket.native());

        this->_socket.shutdown();
        this->_socket.close();
    }
}

// event
void chen::dgram_client::onReadable()
{
    this->recv();
}

void chen::dgram_client::onWritable()
{
    this->send();
}

void chen::dgram_client::onEvent(runloop::Event type)
{
    switch (type)
    {
        case runloop::Event::Readable:
            return this->onReadable();

        case runloop::Event::Writable:
            return this->onWritable();

        default:
            return;
    }
}