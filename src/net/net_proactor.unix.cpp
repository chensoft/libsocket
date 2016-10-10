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
    // todo
}

void chen::net::proactor::write(net::socket *ptr, std::vector<std::uint8_t> &&data)
{
    // todo
}

void chen::net::proactor::write(net::socket *ptr, const std::vector<std::uint8_t> &data)
{
    // todo
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
    }
}

void chen::net::proactor::stop()
{
    this->_model.wake();
}

#endif