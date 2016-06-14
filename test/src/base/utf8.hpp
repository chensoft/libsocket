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
	// if I use Chinese, visual studio 2015 will report error: C2001 newline in constant
	// if I put a char in the end the error will disappear, so I add a dot to each text
    EXPECT_EQ(14, chen::utf8::count(u8"陈剑是libchen的作者."));
    EXPECT_EQ(u8"陈剑-", chen::utf8::substr(u8"陈剑-是libchen的作者.", 0, 3));
    EXPECT_TRUE(chen::utf8::bom(u8"\xEF\xBB\xBF这个字符串有BOM"));
    EXPECT_EQ(u8"陈.", chen::utf8::convert(0x9648) + ".");
}