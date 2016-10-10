/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/net/net_proactor.hpp>
#include <chen/sys/sys.hpp>
#include <socket/bsd/bsd_kqueue.hpp>

// -----------------------------------------------------------------------------
// proactor

// data
void chen::net::proactor::read(net::socket *ptr, std::size_t size)
{
    auto &queue = this->_read[ptr];

    if (!queue.empty())
    {
        // wait for read event
        queue.emplace(net::message(size));
    }
    else
    {
        // todo
    }
}

void chen::net::proactor::write(net::socket *ptr, std::vector<std::uint8_t> &&data)
{
    auto &queue = this->_write[ptr];

    if (!queue.empty())
    {
        // wait for write event
        queue.emplace(net::message(std::move(data)));
    }
    else
    {
        // todo
    }
}

void chen::net::proactor::write(net::socket *ptr, const std::vector<std::uint8_t> &data)
{
    auto &queue = this->_write[ptr];

    if (!queue.empty())
    {
        // wait for write event
        queue.emplace(net::message(data));
    }
    else
    {
        // todo
    }
}

void chen::net::proactor::write(net::socket *ptr, std::vector<std::uint8_t> &&data, const bsd::endpoint &ep)
{
    auto &queue = this->_write[ptr];

    if (!queue.empty())
    {
        // wait for write event
        queue.emplace(net::message(std::move(data), ep));
    }
    else
    {
        // todo
    }
}

void chen::net::proactor::write(net::socket *ptr, const std::vector<std::uint8_t> &data, const bsd::endpoint &ep)
{
    auto &queue = this->_write[ptr];

    if (!queue.empty())
    {
        // wait for write event
        queue.emplace(net::message(data, ep));
    }
    else
    {
        // todo
    }
}

void chen::net::proactor::remove(net::socket *ptr)
{
    this->_read.erase(ptr);
    this->_write.erase(ptr);
}

// control
void chen::net::proactor::start()
{
    while (true)
    {
        auto data = this->_model.poll();

        // check whether the user is request to stop proactor
        if (data.ev == model::Event::None)
            break;

        // retrieve the socket pointer associated with event
        auto ptr = this->_map[data.fd];
        if (!ptr)
            throw std::system_error(sys::error(), "proactor: event happened but no related socket");

        // connection refused, disconnect or other error
        // socket may have unread data even if it has already been closed
        if (data.ev == model::Event::End)
        {
            // try to read rest of the data
            // todo

            // clear pending read and write
            this->remove(ptr);

            // notify the final end event
            ptr->onEnd();

            continue;
        }

        // simulate proactor since kqueue & epoll are reactor model
        if (data.ev == model::Event::Read)
        {
            // todo move to another method
            auto &queue = this->_read[ptr];
            if (queue.empty())
                continue;

            auto message = std::move(queue.front());
            auto &buffer = message.buffer();

            queue.pop();

            // todo handle udp
            if (message.origin())
            {
                // read data from remote
                auto len = ptr->handle().recv(buffer.data(), buffer.size());
                ptr->onRead(std::move(buffer), nullptr, len < 0 ? sys::error() : std::error_code());
            }
            else
            {
                // just notify read event
                ptr->onRead({}, nullptr, {});
            }
        }
        else if (data.ev == model::Event::Write)
        {
            auto &queue = this->_write[ptr];
            if (queue.empty())
                continue;

            // todo handle udp
            auto &message = queue.front();
            auto &buffer  = message.buffer();
            auto length   = ptr->handle().send(buffer.data(), buffer.size());

            if (length >= 0)
            {
                buffer.resize(buffer.size() - length);

                if (buffer.empty())
                {
                    // all data have been sent
                    auto origin = message.origin();

                    queue.pop();

                    ptr->onWrite(origin, {}, {});
                }
                else
                {
                    // write buffer is full
                    // just wait for next write event
                }
            }
            else
            {
                ptr->onWrite(0, {}, sys::error());
            }
        }
        else
        {
            throw std::runtime_error("proactor: event happened but type is unknown");
        }
    }
}

void chen::net::proactor::stop()
{
    this->_model.wake();
}

#endif