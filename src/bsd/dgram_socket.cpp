/**
 * Created by Jian Chen
 * @since  2016.11.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/bsd/dgram_socket.hpp>
#include <cstring>

// -----------------------------------------------------------------------------
// dgram_socket

// read
void chen::dgram_socket::read(std::size_t size)
{
    this->_receive.resize(size);
    this->recv();
}

// write
void chen::dgram_socket::write(const char *text, const basic_address &addr)
{
    this->write(text, ::strlen(text), addr);
}

void chen::dgram_socket::write(const std::string &text, const basic_address &addr)
{
    this->write(text.data(), text.size(), addr);
}

void chen::dgram_socket::write(const void *data, std::size_t size, const basic_address &addr)
{
    auto ptr = static_cast<const std::uint8_t*>(data);

    // if pending data exist, it means we are waiting for writable event
    if (!this->_pending.empty())
        return this->_pending.emplace_back(dgram_packet(std::vector<std::uint8_t>(ptr, ptr + size), addr));

    // try to use send directly first
    if (this->send(data, size, addr))
        this->notify(dgram_write_event(ptr, size, addr));
    else
        this->_pending.emplace_back(dgram_packet(std::vector<std::uint8_t>(ptr, ptr + size), addr));
}

// event
void chen::dgram_socket::attach(std::function<void (dgram_read_event event)> callback)
{
    this->_cb_read = callback;
}

void chen::dgram_socket::attach(std::function<void (dgram_write_event event)> callback)
{
    this->_cb_write = callback;
}

void chen::dgram_socket::notify(dgram_read_event &&event)
{
    if (this->_cb_read)
        this->_cb_read(std::move(event));
}

void chen::dgram_socket::notify(dgram_write_event &&event)
{
    if (this->_cb_write)
        this->_cb_write(std::move(event));
}

// recv
void chen::dgram_socket::recv()
{
    if (this->_receive.empty())
        return;

    basic_address addr;

    auto len = this->_socket.recvfrom(this->_receive.data(), this->_receive.size(), addr);
    if (len < 0)
        return;

    this->_receive.resize(static_cast<std::size_t>(len));
    this->notify(dgram_read_event(std::move(this->_receive), addr));
}

// send
void chen::dgram_socket::send()
{
    while (!this->_pending.empty())
    {
        auto &front = this->_pending.front();
        if (!this->send(front.data.data(), front.data.size(), front.addr))
            break;

        auto move = std::move(front);

        this->_pending.erase(this->_pending.begin());
        this->notify(dgram_write_event(std::move(move)));
    }
}

bool chen::dgram_socket::send(const void *data, std::size_t size, const basic_address &addr)
{
    return this->_socket.sendto(data, size, addr) >= 0;
}