/**
 * Created by Jian Chen
 * @since  2015.12.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>

namespace chen
{
    class num
    {
    public:
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
         * To string(cross-platform)
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
    };
}