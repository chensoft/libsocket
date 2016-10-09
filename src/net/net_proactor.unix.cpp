/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <socket/net/net_proactor.hpp>

// -----------------------------------------------------------------------------
// proactor
chen::net::proactor::proactor()
{
}

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
    // todo
}

// control
void chen::net::proactor::start()
{
    // todo
}

void chen::net::proactor::stop()
{
    this->_model.wake();
}

#endif