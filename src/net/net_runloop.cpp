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
void chen::net::runloop::set(socket_t fd, int opcode, callback_type callback)
{
    this->set(fd, opcode, 0, callback);
}

void chen::net::runloop::set(socket_t fd, int opcode, int flag, callback_type callback)
{
    this->_reactor.set(fd, opcode, flag);
    this->_callbacks[fd] = callback;
}

void chen::net::runloop::del(socket_t fd)
{
    this->_reactor.del(fd);
    this->_callbacks.erase(fd);
}

// control
void chen::net::runloop::start()
{

}

void chen::net::runloop::stop()
{
    this->_reactor.stop();
}