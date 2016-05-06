/**
 * Created by Jian Chen
 * @since  2015.12.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "num.hpp"
#include <algorithm>

using namespace chen;

// -----------------------------------------------------------------------------
// num

// swap
std::int16_t num::swap(std::int16_t value)
{
    return static_cast<std::int16_t>(num::swap(static_cast<std::uint16_t>(value)));
}

std::int32_t num::swap(std::int32_t value)
{
    return static_cast<std::int32_t>(num::swap(static_cast<std::uint32_t>(value)));
}

std::int64_t num::swap(std::int64_t value)
{
    return static_cast<std::int64_t>(num::swap(static_cast<std::uint64_t>(value)));
}

std::uint16_t num::swap(std::uint16_t value)
{
    return (value << 8) | (value >> 8);
}

std::uint32_t num::swap(std::uint32_t value)
{
    std::uint8_t *ptr = reinterpret_cast<std::uint8_t*>(&value);
    std::reverse(ptr, ptr + sizeof(value));
    return value;
}

std::uint64_t num::swap(std::uint64_t value)
{
    std::uint8_t *ptr = reinterpret_cast<std::uint8_t*>(&value);
    std::reverse(ptr, ptr + sizeof(value));
    return value;
}