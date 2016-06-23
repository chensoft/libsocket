/**
 * Created by Jian Chen
 * @since  2016.06.17
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(DataJsonTest, Type)
{
    EXPECT_TRUE(chen::json().isNone());
    EXPECT_TRUE(chen::json(chen::json::object()).isObject());
    EXPECT_TRUE(chen::json(chen::json::array()).isArray());
    EXPECT_TRUE(chen::json(0.715002586).isNumber());
    EXPECT_TRUE(chen::json("Jian Chen").isString());
    EXPECT_TRUE(chen::json(true).isTrue());
    EXPECT_TRUE(chen::json(false).isFalse());
    EXPECT_TRUE(chen::json(nullptr).isNull());
}

TEST(DataJsonTest, Validate)
{
    if (conf::data.empty())
        return ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: you didn't specify test data folder, skip json test\n\n");

    // fail
    for (int i = 1; i <= 33; ++i)
    {
        if (i == 18)  // I don't think too deep is an error
            continue;

        EXPECT_THROW(chen::json::validate(chen::fs::read(conf::data + chen::str::format("/json/fail%d.json", i))), chen::json::error);
    }

    // pass
    for (int j = 1; j <= 3; ++j)
    {
        EXPECT_NO_FATAL_FAILURE(chen::json::validate(chen::fs::read(conf::data + chen::str::format("/json/pass%d.json", j))));
    }
}