/**
 * Created by Jian Chen
 * @since  2015.12.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <algorithm>
#include <limits>
#include <string>

namespace chen
{
    class num
    {
    public:
        /**
         * Test equality
         */
        static bool equal(float a, float b, float epsilon = std::numeric_limits<float>::epsilon());
        static bool equal(double a, double b, double epsilon = std::numeric_limits<double>::epsilon());
        static bool equal(long double a, long double b, long double epsilon = std::numeric_limits<long double>::epsilon());

        /**
         * Swap integer
         */
        template <typename IntType>
        static IntType swap(IntType value)
        {
            std::reverse(reinterpret_cast<std::uint8_t*>(&value), reinterpret_cast<std::uint8_t*>(&value) + sizeof(value));
            return value;
        }

        /**
         * To string(just for compatibility, some platforms don't support std::to_string yet)
         */
        static std::string str(bool value);

        static std::string str(int value);
        static std::string str(long value);
        static std::string str(long long value);

        static std::string str(unsigned int value);
        static std::string str(unsigned long value);
        static std::string str(unsigned long long value);

        static std::string str(float value);
        static std::string str(double value);
        static std::string str(long double value);

        /**
         * Bits of one in the value
         */
        static int bits(std::uint32_t value);

        /**
         * Get a random number between a range
         * [lower, upper) if number is double
         * [lower, upper] if number is int
         */
        static double random(double lower = 0, double upper = 1);
        static int random(int lower, int upper);

        /**
         * check whether an integer is a prime number
         */
        static bool prime(std::uint64_t value);
    };
}