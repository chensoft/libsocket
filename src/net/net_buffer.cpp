/**
 * Created by Jian Chen
 * @since  2016.10.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_buffer.hpp>

// -----------------------------------------------------------------------------
// buffer
chen::net::buffer::buffer(std::size_t size) : _orig(size), _data(size)
{
}

chen::net::buffer::buffer(std::vector<std::uint8_t> &&data) : _orig(data.size()), _data(std::move(data))
{
}

chen::net::buffer::buffer(const std::vector<std::uint8_t> &data) : _orig(data.size()), _data(data)
{
}

// property
std::size_t chen::net::buffer::orig() const
{
    return this->_orig;
}

void chen::net::buffer::orig(std::size_t size)
{
    this->_orig = size;
}

std::vector<std::uint8_t>& chen::net::buffer::data()
{
    return this->_data;
}

const std::vector<std::uint8_t>& chen::net::buffer::data() const
{
    return this->_data;
}