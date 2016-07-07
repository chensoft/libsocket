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

    // these txt files contains utf-8 chars, each char's size is 1byte ~ 6byte
    auto bom   = chen::fs::read(conf::data + "/utf8/bom.txt");
    auto code  = chen::fs::read(conf::data + "/utf8/code.txt");
    auto plain = chen::fs::read(conf::data + "/utf8/plain.txt");
    auto sub   = chen::fs::read(conf::data + "/utf8/sub.txt");

    EXPECT_EQ(11, chen::utf8::count(plain));
    EXPECT_EQ(sub, chen::utf8::substr(plain, 0, 7));
    EXPECT_EQ("", chen::utf8::substr(plain, 5, 0));
    EXPECT_TRUE(chen::utf8::bom(bom));
    EXPECT_FALSE(chen::utf8::bom(plain));
    EXPECT_EQ(code, chen::utf8::convert(0x9648));
}