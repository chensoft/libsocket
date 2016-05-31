/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseUtf8Test, General)
{
    EXPECT_EQ(13, chen::utf8::count("陈剑是libchen的作者"));
    EXPECT_EQ("陈剑", chen::utf8::substr("陈剑是libchen的作者", 0, 2));
    EXPECT_TRUE(chen::utf8::bom("\xEF\xBB\xBF这个字符串有BOM"));
    EXPECT_EQ("陈", chen::utf8::convert(0x9648));
}