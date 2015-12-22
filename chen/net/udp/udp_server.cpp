/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "udp_server.h"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server

void server::start()
{
    {
        // reset quit variable
        std::lock_guard<std::mutex> lock(this->_mutex);
        this->_quit = false;
    }

    // define maximum udp buffer
    // 65535 − 8 byte UDP header − 20 byte IP header = 65507
    // see https://en.wikipedia.org/wiki/User_Datagram_Protocol
    std::size_t length = 65507;
    std::unique_ptr<std::uint8_t> pointer(new std::uint8_t[length]);
    std::uint8_t *buffer = pointer.get();

    while (!this->_quit)
    {
        // receive data from remote
        std::string addr;
        std::uint16_t port = 0;
        std::size_t size = length;

        // set timeout, let server can quit gracefully
        this->recv(buffer, size, addr, port, 1);

        // post result to callback
        if (size)
            this->notify(std::vector<std::uint8_t>(buffer, buffer + size), addr, port);
    }

    this->close();
}

void server::stop()
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_quit = true;
}

void server::close()
{
    socket::close();

    this->_addr.clear();
    this->_port = 0;

    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_quit = true;
}

void server::attach(const callback_type &callback)
{
    this->_callback = callback;
}

void server::detach()
{
    this->_callback = nullptr;
}

void server::notify(const std::vector<std::uint8_t> &data, const std::string &addr, std::uint16_t port)
{
    if (this->_callback)
        this->_callback(data, addr, port);
}

std::string server::addr() const
{
    return this->_addr;
}

std::uint16_t server::port() const
{
    return this->_port;
}