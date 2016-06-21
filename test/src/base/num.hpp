/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseNumTest, General)
{
    // equal
    EXPECT_TRUE(chen::num::equal(0.1, 0.1));
    EXPECT_TRUE(chen::num::equal(0.1f, 0.100000001490116119715002586f));
    EXPECT_TRUE(chen::num::equal(0.1, 0.1000000000000000055511151231257001365183510300715002586));

    // swap
    EXPECT_EQ(static_cast<std::uint16_t>(4660), chen::num::swap(static_cast<std::uint16_t>(13330)));
    EXPECT_EQ(static_cast<std::uint32_t>(305419896), chen::num::swap(static_cast<std::uint32_t>(2018915346)));

    // str
    EXPECT_EQ("true", chen::num::str(true));
    EXPECT_EQ("false", chen::num::str(false));
    EXPECT_EQ("115", chen::num::str(115));
    EXPECT_EQ("-115", chen::num::str(-115));

    // prime
    EXPECT_FALSE(chen::num::prime(0));
    EXPECT_FALSE(chen::num::prime(1));
    EXPECT_TRUE(chen::num::prime(2));
    EXPECT_TRUE(chen::num::prime(3));
    EXPECT_FALSE(chen::num::prime(4));
    EXPECT_TRUE(chen::num::prime(5));
    EXPECT_FALSE(chen::num::prime(6));
    EXPECT_TRUE(chen::num::prime(7));
    EXPECT_FALSE(chen::num::prime(8));
    EXPECT_FALSE(chen::num::prime(9));
}