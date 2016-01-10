/**
 * Created by Jian Chen
 * @since  2015.12.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "num.hpp"
#include <algorithm>
#include <sstream>
#include <cstdlib>

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

// str
std::string num::str(bool value)
{
    return value ? "true" : "false";
}

std::string num::str(int value)
{
    return num::str(static_cast<long long>(value));
}

std::string num::str(long value)
{
    return num::str(static_cast<long long>(value));
}

std::string num::str(long long value)
{
    std::string temp;
    bool sign = false;

    if (value < 0)
    {
        sign  = true;
        value = -value;
    }

    do
    {
        temp = static_cast<char>((value % 10 + '0')) + temp;
    } while ((value /= 10) > 0);

    if (sign)
        temp = "-" + temp;

    return temp;
}

std::string num::str(unsigned int value)
{
    return num::str(static_cast<unsigned long long>(value));
}

std::string num::str(unsigned long value)
{
    return num::str(static_cast<unsigned long long>(value));
}

std::string num::str(unsigned long long value)
{
    std::string temp;

    do
    {
        temp = static_cast<char>((value % 10 + '0')) + temp;
    } while ((value /= 10) > 0);

    return temp;
}

std::string num::str(float value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string num::str(double value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string num::str(long double value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}