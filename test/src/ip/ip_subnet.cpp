/**
 * Created by Jian Chen
 * @since  2016.08.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_subnet.hpp>
#include <socket/ip/ip_error.hpp>
#include <gtest/gtest.h>

TEST(IPSubnetTest, IPv4)
{
    // assign
    EXPECT_EQ(chen::ip::subnet_v4(), chen::ip::subnet_v4("0.0.0.0"));

    EXPECT_EQ(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1/8"));
    EXPECT_EQ(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1", 8));
    EXPECT_EQ(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1", "255.0.0.0"));

    EXPECT_EQ(chen::ip::subnet_v4("127"), chen::ip::subnet_v4("127.0.0.0"));
    EXPECT_EQ(chen::ip::subnet_v4("127.1"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_EQ(chen::ip::subnet_v4("192.168.1"), chen::ip::subnet_v4("192.168.0.1"));

    EXPECT_EQ(chen::ip::subnet_v4(0x7F000001), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_EQ(chen::ip::subnet_v4(0x7F000001, 8), chen::ip::subnet_v4("127.0.0.1/8"));
    EXPECT_EQ(chen::ip::subnet_v4(0x7F000001, "255.0.0.0"), chen::ip::subnet_v4("127.0.0.1/8"));

    EXPECT_THROW(chen::ip::subnet_v4("127.0.0.1", 33), chen::ip::error_subnet);
    EXPECT_THROW(chen::ip::subnet_v4(0x7F000001, 33), chen::ip::error_subnet);

    chen::ip::subnet_v4 v4("127");
    EXPECT_EQ("127.0.0.0/32", v4.str());

    v4 = "192.168.1.1/24";
    EXPECT_EQ("192.168.1.1/24", v4.str());
    EXPECT_EQ(24, v4.cidr());

    v4 = 0x7F000001;
    EXPECT_EQ("127.0.0.1/32", v4.str());
    EXPECT_EQ(32, v4.cidr());

    // representation
    EXPECT_EQ("127.0.0.1/32", chen::ip::subnet_v4("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1/8", chen::ip::subnet_v4("127.0.0.1/8").str());
    EXPECT_EQ("127.0.0.1/8", chen::ip::subnet_v4(0x7F000001, 8).str());
    EXPECT_EQ("192.168.1.1/30", chen::ip::subnet_v4("192.168.1.1", 30).str());

    // network
    EXPECT_EQ(8, chen::ip::subnet_v4("127.0.0.1/8").cidr());
    EXPECT_EQ(16, chen::ip::subnet_v4("180.166.92.36/16").cidr());
    EXPECT_EQ(24, chen::ip::subnet_v4("192.168.0.1/24").cidr());
    EXPECT_EQ(26, chen::ip::subnet_v4("192.168.0.1/26").cidr());

    EXPECT_EQ(chen::ip::subnet_v4("255.0.0.0").addr(), chen::ip::subnet_v4("127.0.0.1/8").netmask());
    EXPECT_EQ(chen::ip::subnet_v4("0.255.255.255").addr(), chen::ip::subnet_v4("127.0.0.1/8").wildcard());

    EXPECT_EQ("127.0.0.0/8", chen::ip::subnet_v4("127.0.0.1/8").network().str());
    EXPECT_EQ("127.0.0.1/8", chen::ip::subnet_v4("127.0.0.1/8").minhost().str());
    EXPECT_EQ("127.255.255.254/8", chen::ip::subnet_v4("127.0.0.1/8").maxhost().str());
    EXPECT_EQ("127.255.255.255/8", chen::ip::subnet_v4("127.0.0.1/8").broadcast().str());
    EXPECT_EQ(16777214, chen::ip::subnet_v4("127.0.0.1/8").hosts());

    // special
    EXPECT_TRUE(chen::ip::subnet_v4("192.168.0.255/24").isBroadcast());
    EXPECT_FALSE(chen::ip::subnet_v4("192.168.0.1/24").isBroadcast());

    // operator
    EXPECT_EQ(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_EQ(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1/8"));

    EXPECT_NE(chen::ip::subnet_v4("127.0.0.0"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_NE(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1"));

    EXPECT_LT(chen::ip::subnet_v4("127.0.0.0"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_LT(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1"));

    EXPECT_GT(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.0"));
    EXPECT_GT(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.1/8"));

    EXPECT_LE(chen::ip::subnet_v4("127.0.0.0"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_LE(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_LE(chen::ip::subnet_v4("127.0.0.1/8"), chen::ip::subnet_v4("127.0.0.1"));

    EXPECT_GE(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.0"));
    EXPECT_GE(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.1"));
    EXPECT_GE(chen::ip::subnet_v4("127.0.0.1"), chen::ip::subnet_v4("127.0.0.1/8"));
}