/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/base/num.hpp>
#include <gtest/gtest.h>

TEST(BaseNumTest, General)
{
    // equal
    EXPECT_TRUE(chen::num::equal(0.1, 0.1));
    EXPECT_TRUE(chen::num::equal(0.1f, 0.100000001490116119715002586f));
    EXPECT_TRUE(chen::num::equal(0.1, 0.1000000000000000055511151231257001365183510300715002586));
    EXPECT_TRUE(chen::num::equal(0.1L, 0.1000000000000000000011151231257001365183510300715002586L));

    // swap
    EXPECT_EQ(4660, chen::num::swap(static_cast<std::int16_t>(13330)));
    EXPECT_EQ(305419896, chen::num::swap(static_cast<std::int32_t>(2018915346)));
    EXPECT_EQ(601138196360724480, chen::num::swap(static_cast<std::int64_t>(201891534600)));
    EXPECT_EQ(static_cast<std::uint16_t>(4660), chen::num::swap(static_cast<std::uint16_t>(13330)));
    EXPECT_EQ(static_cast<std::uint32_t>(305419896), chen::num::swap(static_cast<std::uint32_t>(2018915346)));
    EXPECT_EQ(static_cast<std::uint64_t>(601138196360724480), chen::num::swap(static_cast<std::uint64_t>(201891534600)));

    // str
    EXPECT_EQ("true", chen::num::str(true));
    EXPECT_EQ("false", chen::num::str(false));
    EXPECT_EQ("115", chen::num::str(115));
    EXPECT_EQ("115", chen::num::str(115u));
    EXPECT_EQ("-115", chen::num::str(-115));

    EXPECT_EQ("115", chen::num::str(115.0f));  // because I use %g format
    EXPECT_EQ("115", chen::num::str(115.0));
    EXPECT_EQ("115", chen::num::str(115.0L));

    auto rand_real = chen::num::random(0.0, 2.0);
    auto rand_int  = chen::num::random(1, 9999);

    EXPECT_GE(rand_real, 0);
    EXPECT_LT(rand_real, 2);

    EXPECT_GE(rand_int, 1);
    EXPECT_LE(rand_int, 9999);

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