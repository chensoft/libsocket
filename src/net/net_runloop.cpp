/**
 * Created by Jian Chen
 * @since  2016.10.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_runloop.hpp>

// -----------------------------------------------------------------------------
// runloop
const int chen::net::runloop::FlagOnce = chen::bsd::reactor::FlagOnce;
const int chen::net::runloop::FlagEdge = chen::bsd::reactor::FlagEdge;

// modify
void chen::net::runloop::set(net::socket *ptr, int opcode, callback_type callback)
{
    this->set(ptr, opcode, 0, callback);
}

void chen::net::runloop::set(net::socket *ptr, int opcode, int flag, callback_type callback)
{
    this->_reactor.set(ptr->handle().native(), opcode, flag);
    this->_callbacks[ptr] = callback;
}

void chen::net::runloop::del(net::socket *ptr)
{
    this->_reactor.del(ptr->handle().native());
    this->_callbacks.erase(ptr);
}

// control
void chen::net::runloop::start()
{

}

void chen::net::runloop::stop()
{
    this->_reactor.stop();
}