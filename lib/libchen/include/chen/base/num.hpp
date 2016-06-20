/**
 * Created by Jian Chen
 * @since  2015.12.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
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
        static std::int16_t swap(std::int16_t value);
        static std::int32_t swap(std::int32_t value);
        static std::int64_t swap(std::int64_t value);

        static std::uint16_t swap(std::uint16_t value);
        static std::uint32_t swap(std::uint32_t value);
        static std::uint64_t swap(std::uint64_t value);

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
         * Get a random number between a range
         * [lower, upper) if number is double
         * [lower, upper] if number is int
         */
        static double random(double lower = 0, double upper = 1);
        static int random(int lower, int upper);
    };
}