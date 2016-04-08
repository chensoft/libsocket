/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "udp_server.hpp"
#include "udp_define.hpp"

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server

void server::start()
{
    {
        // reset quit variable
        std::lock_guard<std::mutex> lock(this->_mutex);

        if (this->_quit)
        {
            this->_quit = false;
            return;
        }
    }

    // define maximum udp buffer
    std::size_t length = SIZE_LIMIT_PACKET;
    std::unique_ptr<std::uint8_t> pointer(new std::uint8_t[length]);
    std::uint8_t *buffer = pointer.get();

    while (!this->_quit)
    {
        // receive data from remote
        std::string addr;
        std::uint16_t port = 0;
        std::size_t size = length;

        // set timeout, let server quit gracefully
        this->recv(buffer, size, addr, port, 1);

        // post result to callback
        if (size)
            this->notify(std::vector<std::uint8_t>(buffer, buffer + size), std::move(addr), port);
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

void server::notify(std::vector<std::uint8_t> data, std::string addr, std::uint16_t port)
{
    if (this->_callback)
        this->_callback(std::move(data), std::move(addr), port);
}

std::string server::addr() const
{
    return this->_addr;
}

std::uint16_t server::port() const
{
    return this->_port;
}