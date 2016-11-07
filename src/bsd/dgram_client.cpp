/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/dgram_handler.hpp>

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

// read
void chen::dgram_client::read(std::size_t size)
{
    this->_reading = size;
    this->receive();
}

// write
void chen::dgram_client::write(const char *text, const basic_address &addr)
{
    this->write(text, ::strlen(text), addr);
}

void chen::dgram_client::write(const std::string &text, const basic_address &addr)
{
    this->write(text.data(), text.size(), addr);
}

void chen::dgram_client::write(const void *data, std::size_t size, const basic_address &addr)
{
    auto ptr = static_cast<const std::uint8_t*>(data);

    // if pending data exist, it means we are waiting for writable event
    if (!this->_buf_write.empty())
        return this->_buf_write.emplace_back(dgram_packet(std::vector<std::uint8_t>(ptr, ptr + size), addr));

    // try to use send directly first
    auto ret = this->_socket.sendto(data, size, addr);
    auto len = static_cast<std::size_t>(ret >= 0 ? ret : 0);

    if (len == size)
    {
        // all data has been sent out
        this->notify(len, addr);
    }
    else
    {
        // store packet to buffer
        this->_buf_write.emplace_back(dgram_packet(std::vector<std::uint8_t>(ptr, ptr + size), addr));
    }
}

// handler
void chen::dgram_client::attach(dgram_client_handler *handler)
{
    this->_handler = handler;
}

void chen::dgram_client::detach()
{
    this->_handler = nullptr;
}

void chen::dgram_client::notify(std::vector<std::uint8_t> &&data, const chen::basic_address &addr)
{
    if (this->_handler)
        this->_handler->onRead(*this, std::move(data), std::move(addr));
}

void chen::dgram_client::notify(std::size_t size, const chen::basic_address &addr)
{
    if (this->_handler)
        this->_handler->onWrite(*this, size, addr);
}

// event
void chen::dgram_client::receive()
{
    if (!this->_reading)
        return;

    // read one packet data
    std::vector<std::uint8_t> buf(this->_reading);
    basic_address addr;

    auto len = this->_socket.recvfrom(buf.data(), this->_reading, addr);
    if (len < 0)
        return;

    buf.resize(static_cast<std::size_t>(len));

    this->notify(std::move(buf), addr);
}

void chen::dgram_client::onReadable()
{
    this->receive();
}

void chen::dgram_client::onWritable()
{
    // try to send all data
    while (!this->_buf_write.empty())
    {
        auto &front = this->_buf_write.front();

        auto ret = this->_socket.sendto(front.data.data(), front.data.size(), front.addr);
        if (ret <= 0)
            break;

        // notify write callback
        auto addr = front.addr;

        this->_buf_write.erase(this->_buf_write.begin());
        this->notify(static_cast<std::size_t>(ret), addr);
    }
}

void chen::dgram_client::onEnded()
{
}

void chen::dgram_client::onEvent(runloop::Event type)
{
    switch (type)
    {
        case runloop::Event::Readable:
            return this->onReadable();

        case runloop::Event::Writable:
            return this->onWritable();

        case runloop::Event::Ended:
            return this->onEnded();
    }
}