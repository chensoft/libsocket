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
    this->_mapping[fd] = callback;
}

void chen::net::runloop::del(socket_t fd)
{
    this->_reactor.del(fd);
    this->_mapping.erase(fd);
}

// control
void chen::net::runloop::start(std::size_t count, double timeout)
{
    // todo fix if user call del() method in callback, cause multiple events handle maybe error
    this->_caching.resize(count);

    while (true)
    {
        auto num = this->_reactor.poll(this->_caching, count, timeout);
        if (!num)
            break;  // user request to stop or timeout

        for (std::size_t idx = 0; idx < num; ++idx)
        {
            auto &event = this->_caching[idx];

            if (event.ev == bsd::reactor::Event::None)
                throw std::runtime_error("runloop: unknown event type detect");

            auto find = this->_mapping.find(event.fd);
            if (find == this->_mapping.end())
                throw std::runtime_error("runloop: event detect but no callback");

            find->second(event.ev);
        }
    }
}

void chen::net::runloop::stop()
{
    this->_reactor.stop();
}