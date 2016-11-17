/**
 * Created by Jian Chen
 * @since  2016.10.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// reactor

// modify
void chen::reactor::set(handle_t fd, int opcode, callback_type callback)
{
    this->set(fd, opcode, 0, callback);
}

void chen::reactor::set(handle_t fd, int opcode, int flag, callback_type callback)
{
    this->_backend.set(fd, opcode, flag);
    this->_mapping[fd] = callback;
}

void chen::reactor::del(handle_t fd)
{
    // unregister event and callback
    this->_backend.del(fd);
    this->_mapping.erase(fd);

    // deactivate fd's unhandled events, if user delete
    // socket in previous callback, then the next for
    // loop will access a invalid socket's event
    for (std::size_t i = 0; i < this->_count; ++i)
    {
        auto &event = this->_caching[i];
        if (event.fd == fd)
            event.fd = invalid_handle;
    }
}

// control
void chen::reactor::run(std::size_t count, double timeout)
{
    this->_caching.resize(count);

    while (true)
    {
        this->_count = this->_backend.poll(this->_caching, count, timeout);
        if (!this->_count)
            break;  // user request to stop or timeout

        for (std::size_t idx = 0; idx < this->_count; ++idx)
        {
            auto &event = this->_caching[idx];
            if (event.fd == invalid_handle)
                continue;  // someone has removed the socket in previous callback

            auto find = this->_mapping.find(event.fd);
            if (find == this->_mapping.end())
                throw std::runtime_error("reactor: event detect but no callback");

            // remove fd if Ended event occurs
            auto callback = find->second;

            if (event.ev == Event::Ended)
                this->del(event.fd);

            callback(event.ev);
        }

        this->_count = 0;
    }
}

void chen::reactor::stop()
{
    this->_backend.stop();
}
