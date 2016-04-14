/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "tcp_server.hpp"
#include <chen/net/so/so_error.hpp>

using namespace chen;
using namespace chen::tcp;

// -----------------------------------------------------------------------------
// server
void server::start()
{
    // check if bind
    if (!this->localPort())
        throw so::error_bind("tcp socket not bind");

    // listen
    this->listen();

    // accept
    while (true)
    {
        auto conn = this->accept();

        if (conn)
            this->notify(std::move(conn));
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

void server::notify(std::unique_ptr<chen::tcp::conn> conn)
{
    if (this->_callback)
        this->_callback(std::move(conn));
}
