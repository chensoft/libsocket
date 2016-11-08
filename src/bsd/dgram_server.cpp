/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/dgram_server.hpp>

// -----------------------------------------------------------------------------
// dgram_server
chen::dgram_server::dgram_server(runloop &loop, const basic_address &addr, int protocol) : _runloop(loop), _local(addr)
{
    this->_socket.reset(addr.addr.ss_family, SOCK_DGRAM, protocol);
    this->_socket.nonblocking(true);

    this->_runloop.set(this->_socket.native(), runloop::OpcodeRW, runloop::FlagEdge,
                       std::bind(&dgram_server::onEvent, this, std::placeholders::_1));
}

chen::dgram_server::~dgram_server()
{
    this->close();
}

// control
void chen::dgram_server::listen(int backlog)
{
    auto err = this->_socket.bind(this->_local);
    if (err)
        throw std::system_error(err, "dgram: server bind error");

    err = this->_socket.listen(backlog);
    if (err)
        throw std::system_error(err, "dgram: server listen error");
}

void chen::dgram_server::close()
{
    if (this->_socket)
    {
        this->_runloop.del(this->_socket.native());

        this->_socket.shutdown();
        this->_socket.close();
    }
}

// property
chen::basic_address chen::dgram_server::local() const
{
    return this->_local;
}

// event
void chen::dgram_server::onReadable()
{
    this->recv();
}

void chen::dgram_server::onWritable()
{
    this->send();
}

void chen::dgram_server::onEvent(runloop::Event type)
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