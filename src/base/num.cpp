/**
 * Created by Jian Chen
 * @since  2015.12.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/base/num.hpp"
#include "chen/base/str.hpp"
#include <random>

// -----------------------------------------------------------------------------
// num

// equal
bool chen::num::equal(float a, float b, float epsilon)
{
    return std::fabs(a - b) < epsilon;
}

bool chen::num::equal(double a, double b, double epsilon)
{
    return std::fabs(a - b) < epsilon;
}

bool chen::num::equal(long double a, long double b, long double epsilon)
{
    return std::fabs(a - b) < epsilon;
}

// str
std::string chen::num::str(bool value)
{
    return value ? "true" : "false";
}

std::string chen::num::str(int value)
{
    return num::str(static_cast<long long>(value));
}

std::string chen::num::str(long value)
{
    return num::str(static_cast<long long>(value));
}

std::string chen::num::str(long long value)
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

std::string chen::num::str(unsigned int value)
{
    return num::str(static_cast<unsigned long long>(value));
}

std::string chen::num::str(unsigned long value)
{
    return num::str(static_cast<unsigned long long>(value));
}

std::string chen::num::str(unsigned long long value)
{
    std::string temp;

    do
    {
        temp = static_cast<char>((value % 10 + '0')) + temp;
    } while ((value /= 10) > 0);

    return temp;
}

std::string chen::num::str(float value)
{
    return str::format("%g", value);
}

std::string chen::num::str(double value)
{
    return str::format("%g", value);
}

std::string chen::num::str(long double value)
{
    return str::format("%Lg", value);
}

// bits
int chen::num::bits(std::uint32_t value)
{
    // @see http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
    value = value - ((value >> 1) & 0x55555555);
    value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
    return (((value + (value >> 4)) & 0x0f0f0f0f) * 0x01010101) >> 24;
}

// random
double chen::num::random(double lower, double upper)
{
    std::random_device device;
    std::mt19937 engine(device());

    std::uniform_real_distribution<double> uniform(lower, upper);
    return uniform(engine);
}

int chen::num::random(int lower, int upper)
{
    std::random_device device;
    std::mt19937 engine(device());

    std::uniform_int_distribution<int> uniform(lower, upper);
    return uniform(engine);
}

// prime
bool chen::num::prime(std::uint64_t value)
{
    if (value <= 1)  // exclude 0 and 1
        return false;

    std::uint64_t i = 2;
    std::uint64_t c = 0;

    while (i <= value / 2)
    {
        if (value % i == 0)
        {
            ++c;
            break;
        }

        ++i;
    }

    return !c;
}