/**
 * Created by Jian Chen
 * @since  2016.06.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(DataIniTest, General)
{
    if (conf::data.empty())
        return ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: you didn't specify test data folder, skip ini test\n\n");

    // fail
    for (int i = 1; i <= 5; ++i)
    {
        EXPECT_THROW(chen::ini::parse(chen::fs::read(conf::data + chen::str::format("/ini/fail%d.ini", i))), chen::ini::error);
    }

    // pass
    for (int j = 1; j <= 4; ++j)
    {
        EXPECT_NO_THROW(chen::ini::parse(conf::data + chen::str::format("/ini/pass%d.ini", j), true));
    }

    // equal(usage of block text in C++11)
    auto text  = R"([section]
key="simple \\\0\a\b\t\r\n\:\"\;\#value")";

    auto parse = chen::ini::parse(text);
    EXPECT_EQ(text, chen::ini::stringify(parse));

    // parse special string
    EXPECT_TRUE(chen::ini::parse("   ").empty());
    EXPECT_THROW(chen::ini::parse("ab=\\x9A"), chen::ini::error);
    EXPECT_THROW(chen::ini::parse("ab=\\x9A@@"), chen::ini::error);
    EXPECT_THROW(chen::ini::parse("emoji=\\xD83D\\xDE00"), chen::ini::error);
}