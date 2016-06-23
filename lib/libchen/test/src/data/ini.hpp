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
        EXPECT_NO_FATAL_FAILURE(chen::ini::parse(chen::fs::read(conf::data + chen::str::format("/ini/pass%d.ini", j))));
    }
}