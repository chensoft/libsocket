/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

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

TEST(BaseRegexTest, General)
{
    try
    {
        EXPECT_TRUE(chen::regex::match("12345", "^\\d+$"));
        EXPECT_EQ("chenjian", chen::regex::replace("chensoft.com", "soft\\.com", "jian"));

        auto group = chen::regex::group("127.0.0.1", "(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)");
        EXPECT_EQ(5, group.size());
        EXPECT_EQ("127", group[1]);
        EXPECT_EQ("0", group[2]);
        EXPECT_EQ("0", group[3]);
        EXPECT_EQ("1", group[4]);
    }
    catch (const std::exception &e)
    {
        ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: regex support is incomplete on your compiler\n\n");
    }
}