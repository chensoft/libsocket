/**
 * Created by Jian Chen
 * @since  2016.09.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "net_proactor.kqueue.hpp"

// -----------------------------------------------------------------------------
// proactor
chen::net::proactor::proactor() : _detail(new proactor_detail)
{
}

chen::net::proactor::~proactor()
{
    delete this->_detail;
}

void chen::net::proactor::send(net::socket *ptr, std::vector<std::uint8_t> &&data)
{
    this->_detail->send(ptr, std::move(data));
}

void chen::net::proactor::recv(net::socket *ptr, std::size_t size)
{
    this->_detail->recv(ptr, size);
}

void chen::net::proactor::remove(net::socket *ptr)
{
    this->_detail->remove(ptr);
}

void chen::net::proactor::start()
{
    this->_detail->start();
}

void chen::net::proactor::stop()
{
    this->_detail->stop();
}