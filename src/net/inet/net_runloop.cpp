/**
 * Created by Jian Chen
 * @since  2016.10.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/inet/net_runloop.hpp>

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
    this->_mapping[fd] = callback;
}

void chen::net::runloop::del(socket_t fd)
{
    // unregister event and callback
    this->_reactor.del(fd);
    this->_mapping.erase(fd);

    // deactivate current fd's event
    for (std::size_t i = 0; i < this->_count; ++i)
    {
        auto &event = this->_caching[i];
        if (event.fd == fd)
            event.fd = invalid_socket;
    }
}

// control
void chen::net::runloop::run(std::size_t count, double timeout)
{
    this->_caching.resize(count);

    while (true)
    {
        this->_count = this->_reactor.poll(this->_caching, count, timeout);
        if (!this->_count)
            break;  // user request to stop or timeout

        for (std::size_t idx = 0; idx < this->_count; ++idx)
        {
            auto &event = this->_caching[idx];
            if (event.fd == invalid_socket)
                continue;  // someone has removed the socket in previous callback

            auto find = this->_mapping.find(event.fd);
            if (find == this->_mapping.end())
                throw std::runtime_error("runloop: event detect but no callback");

            find->second(event.ev);
        }

        this->_count = 0;
    }
}

void chen::net::runloop::stop()
{
    this->_reactor.stop();
}