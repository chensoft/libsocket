/**
 * Created by Jian Chen
 * @since  2016.10.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_runloop.hpp>
#include <chen/time/time.hpp>

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
    // todo allocate events' memory as member data
    // todo fix if user call del() method in callback, cause multiple events handle maybe error
    this->_caching.resize(static_cast<std::size_t>(count));

    while (true)
    {
        chen::time t(true);

        for (int i = 0; i < 10000000; ++i)
        {
            auto num = this->_reactor.fetch(this->_caching, timeout);
            if (!num)
                break;  // user request to stop or timeout

            for (int idx = 0; idx < num; ++idx)
            {
                auto &event = this->_caching[idx];

                if (event.ev == bsd::reactor::Event::None)
                    throw std::runtime_error("runloop: unknown event type detect");

                auto find = this->_mapping.find(event.fd);
                if (find == this->_mapping.end())
                    throw std::runtime_error("runloop: event detect but no callback");

//            find->second(event.ev);
            }
        }

        t.stop();
        printf("xxx: %f\n", t.elapsed());
        return;
    }
}

void chen::net::runloop::stop()
{
    this->_reactor.stop();
}