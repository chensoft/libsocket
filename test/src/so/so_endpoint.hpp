/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/so/so_endpoint.hpp>
#include <gtest/gtest.h>

TEST(SOEndpointTest, General)
{
    EXPECT_EQ(80, chen::so::endpoint("127.0.0.1", 80).port());

    EXPECT_EQ(chen::ip::address_v4("127.0.0.1"), chen::so::endpoint("127.0.0.1", 80).addr());
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1"), chen::so::endpoint(chen::ip::address_v4("127.0.0.1"), 80).addr());

    EXPECT_EQ(chen::so::endpoint("127.0.0.1", 80), chen::so::endpoint("127.0.0.1", 80));
    EXPECT_NE(chen::so::endpoint("192.168.0.1", 80), chen::so::endpoint("127.0.0.1", 80));

    EXPECT_LT(chen::so::endpoint("127.0.0.0", 80), chen::so::endpoint("127.0.0.1", 80));
    EXPECT_LT(chen::so::endpoint("127.0.0.1", 53), chen::so::endpoint("127.0.0.1", 80));

    EXPECT_LE(chen::so::endpoint("127.0.0.1", 80), chen::so::endpoint("127.0.0.1", 80));
    EXPECT_LE(chen::so::endpoint("127.0.0.1", 53), chen::so::endpoint("127.0.0.1", 80));

    EXPECT_GT(chen::so::endpoint("127.0.0.1", 80), chen::so::endpoint("127.0.0.0", 80));
    EXPECT_GT(chen::so::endpoint("127.0.0.1", 80), chen::so::endpoint("127.0.0.1", 53));

    EXPECT_GE(chen::so::endpoint("127.0.0.1", 80), chen::so::endpoint("127.0.0.1", 80));
    EXPECT_GE(chen::so::endpoint("127.0.0.1", 80), chen::so::endpoint("127.0.0.1", 53));
}