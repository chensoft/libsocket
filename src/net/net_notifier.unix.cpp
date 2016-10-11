/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/net/net_notifier.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// notifier

// data
void chen::net::notifier::accept(net::socket *ptr)
{

}

void chen::net::notifier::read(net::socket *ptr, std::size_t size)
{
    auto &queue = this->_read[ptr];

    net::message message(size);

    // try to read data
    if (queue.empty() && size)
    {
        if (ptr->type() == SOCK_STREAM)
        {
            if (ptr->handle().recv(message.buffer().data(), size) >= 0)
                return ptr->onRead(std::move(message.buffer()), nullptr, {});
        }
        else if (ptr->type() == SOCK_DGRAM)
        {
            bsd::endpoint ep;

            if (ptr->handle().recvfrom(message.buffer().data(), size, ep) >= 0)
                return ptr->onRead(std::move(message.buffer()), net::endpoint(ep), {});
        }
    }

    // wait for read event
    queue.emplace(std::move(message));
}

void chen::net::notifier::write(net::socket *ptr, std::vector<std::uint8_t> &&data)
{
    auto &queue = this->_write[ptr];
    auto origin = data.size();

    // try to write data
    if (queue.empty())
    {
        auto length = ptr->handle().send(data.data(), origin);

        if (length > 0)
        {
            if (static_cast<std::size_t>(length) == origin)
            {
                // send all data
                return ptr->onWrite(origin, nullptr, {});
            }
            else
            {
                // send partial data
                data.erase(data.begin(), data.begin() + length);
            }
        }
    }

    // wait for write event
    queue.emplace(net::message(origin, std::move(data)));
}

void chen::net::notifier::write(net::socket *ptr, const std::vector<std::uint8_t> &data)
{
    auto &queue = this->_write[ptr];
    auto origin = data.size();
    auto length = (ssize_t)0;

    // try to write data
    if (queue.empty())
    {
        length = ptr->handle().send(data.data(), origin);

        if (length > 0)
        {
            if (static_cast<std::size_t>(length) == origin)
            {
                // send all data
                return ptr->onWrite(origin, nullptr, {});
            }
            else
            {
                // send partial data
            }
        }
    }

    // wait for write event
    queue.emplace(net::message(origin, std::vector<std::uint8_t>(data.begin() + length, data.end())));
}

void chen::net::notifier::write(net::socket *ptr, std::vector<std::uint8_t> &&data, const bsd::endpoint &ep)
{
    auto &queue = this->_write[ptr];
    auto origin = data.size();

    // try to write data
    if (queue.empty())
    {
        auto length = ptr->handle().sendto(data.data(), origin, ep);

        if (length > 0)
        {
            if (static_cast<std::size_t>(length) == origin)
            {
                // send all data
                return ptr->onWrite(origin, ep, {});
            }
            else
            {
                // send partial data
                data.erase(data.begin(), data.begin() + length);
            }
        }
    }

    // wait for write event
    queue.emplace(net::message(origin, std::move(data), ep));
}

void chen::net::notifier::write(net::socket *ptr, const std::vector<std::uint8_t> &data, const bsd::endpoint &ep)
{
    auto &queue = this->_write[ptr];
    auto origin = data.size();
    auto length = (ssize_t)0;

    // try to write data
    if (queue.empty())
    {
        length = ptr->handle().sendto(data.data(), origin, ep);

        if (length > 0)
        {
            if (static_cast<std::size_t>(length) == origin)
            {
                // send all data
                return ptr->onWrite(origin, ep, {});
            }
            else
            {
                // send partial data
            }
        }
    }

    // wait for write event
    queue.emplace(net::message(origin, std::vector<std::uint8_t>(data.begin() + length, data.end()), ep));
}

void chen::net::notifier::remove(net::socket *ptr)
{
    this->_map.erase(ptr->handle().native());
    this->_accept.erase(ptr);
    this->_read.erase(ptr);
    this->_write.erase(ptr);
}

// control
void chen::net::notifier::start()
{
    while (true)
    {
        auto data = this->_model.poll();

        // check whether the user is request to stop notifier
        if (data.ev == bsd::reactor::Event::None)
            break;

        // retrieve the socket pointer associated with event
        auto ptr = this->_map[data.fd];
        if (!ptr)
            throw std::system_error(sys::error(), "notifier: event happened but no related socket");

        // connection refused, disconnect or other error
        // socket may have unread data even if it has already been closed
        if (data.ev == bsd::reactor::Event::End)
        {
            // try to read rest of the data
            // todo

            // clear pending read and write
            this->remove(ptr);

            // notify the final end event
            ptr->onEnd();

            continue;
        }

        // simulate notifier since kqueue & epoll are reactor model
        if (data.ev == bsd::reactor::Event::Read)
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
        else if (data.ev == bsd::reactor::Event::Write)
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
                buffer.erase(buffer.begin(), buffer.begin() + length);

                if (buffer.empty())
                {
                    // all data have been sent
                    auto origin = message.origin();

                    queue.pop();

                    ptr->onWrite(origin, nullptr, {});
                }
                else
                {
                    // write buffer is full
                    // just wait for next write event
                }
            }
            else
            {
                ptr->onWrite(0, nullptr, sys::error());
            }
        }
        else
        {
            throw std::runtime_error("notifier: event happened but type is unknown");
        }
    }
}

void chen::net::notifier::stop()
{
    this->_model.wake();
}

#endif