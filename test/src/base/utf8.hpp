/**
  * Created by Jian Chen
  * @since  2016.05.27
  * @author Jian Chen <admin@chensoft.com>
  * @link   http://chensoft.com
  */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>
#include "../../conf.hpp"

TEST(BaseUtf8Test, General)
{
    if (conf::data.empty())
        return ::testing::internal::ColoredPrintf(::testing::internal::COLOR_YELLOW, "warning: you didn't specify test data folder, skip utf8 test\n\n");

    auto bom   = chen::fs::read(conf::data + "/utf8/bom.txt");
    auto code  = chen::fs::read(conf::data + "/utf8/code.txt");
    auto plain = chen::fs::read(conf::data + "/utf8/plain.txt");

    EXPECT_EQ(13, chen::utf8::count(plain));
    EXPECT_EQ(code, chen::utf8::substr(plain, 0, 1));
    EXPECT_TRUE(chen::utf8::bom(bom));
    EXPECT_FALSE(chen::utf8::bom(plain));
    EXPECT_EQ(code, chen::utf8::convert(0x9648));
}