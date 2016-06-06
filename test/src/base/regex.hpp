/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseRegexTest, General)
{
    // regex support is incomplete in gcc 4.8.x, so I comment out these codes
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4) && (__GNUC_MINOR__ <= 8)
    #pragma message("regex support is incomplete below gcc 4.9, skip it")
#else
    EXPECT_TRUE(chen::regex::match("12345", "^\\d+$"));
    EXPECT_EQ("chenjian", chen::regex::replace("chensoft.com", "soft\\.com", "jian"));

    auto group = chen::regex::group("127.0.0.1", "(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)");
    EXPECT_EQ(5, group.size());
    EXPECT_EQ("127", group[1]);
    EXPECT_EQ("0", group[2]);
    EXPECT_EQ("0", group[3]);
    EXPECT_EQ("1", group[4]);
#endif
}