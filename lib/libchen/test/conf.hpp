/**
 * Created by Jian Chen
 * @since  2016.06.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>

// test data config
class conf
{
public:
    static std::string data;
};


// see googletest gtest.cc
namespace testing
{
    namespace internal
    {
        enum GTestColor
        {
            COLOR_DEFAULT,
            COLOR_RED,
            COLOR_GREEN,
            COLOR_YELLOW
        };

        void ColoredPrintf(GTestColor color, const char* fmt, ...);
    }
}