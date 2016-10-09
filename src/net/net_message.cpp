/**
 * Created by Jian Chen
 * @since  2016.10.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_message.hpp>

// -----------------------------------------------------------------------------
// message
chen::net::message::message(std::size_t size) : _origin(size), _buffer(size)
{
}

chen::net::message::message(std::vector<std::uint8_t> &&buffer) : _origin(buffer.size()), _buffer(std::move(buffer))
{
}

chen::net::message::message(const std::vector<std::uint8_t> &buffer) : _origin(buffer.size()), _buffer(buffer)
{
}

// property
std::size_t chen::net::message::origin() const
{
    return this->_origin;
}

std::vector<std::uint8_t>& chen::net::message::buffer()
{
    return this->_buffer;
}

const std::vector<std::uint8_t>& chen::net::message::buffer() const
{
    return this->_buffer;
}