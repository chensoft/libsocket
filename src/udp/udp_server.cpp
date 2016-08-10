/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/udp/udp_server.hpp>
#include <socket/udp/udp_define.hpp>
#include <socket/so/so_error.hpp>
#include <chen/chen.hpp>

using namespace chen;
using namespace chen::udp;

// -----------------------------------------------------------------------------
// server
void server::start()
{
    // check if bind
    if (!this->localPort())
        throw so::error_bind("udp: socket not bind");

    // maximum udp buffer
    std::size_t length = SIZE_LIMIT_PACKET;
    std::unique_ptr<std::uint8_t> pointer(new std::uint8_t[length]);
    std::uint8_t *buffer = pointer.get();

    while (true)
    {
        // receive data from remote
        std::string addr;
        std::uint16_t port = 0;

        std::size_t size = 0;

        try
        {
            size = this->recv(buffer, length, addr, port);
        }
        catch (const std::exception &e)
        {
            // todo notify recv exception to handler
            PILogE("%s", e.what());
            continue;
        }

        // post result to callback
        if (size)
            this->notify(std::vector<std::uint8_t>(buffer, buffer + size), std::move(addr), port);
        else
            break;
    }
}

void server::start(const std::string &addr, std::uint16_t port)
{
    this->bind(addr, port);
    this->start();
}

void server::stop()
{
    this->shutdown();
    this->close();
}

// callback
void server::attach(callback_type callback)
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