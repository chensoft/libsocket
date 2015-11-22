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
server::server()
{

}

void server::start()
{

}

void server::attach(const callback_type &callback)
{
    this->_callback = callback;
}

void server::detach()
{
    this->_callback = nullptr;
}

void server::notify()
{
    // todo @@
//    if (this->_callback)
//        this->_callback();
}

std::string server::addr() const
{
    return this->_addr;
}

std::uint16_t server::port() const
{
    return this->_port;
}