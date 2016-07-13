/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/socket.hpp>
#include <gtest/gtest.h>

TEST(IPAddressTest, IPv4)
{
    // assign
    EXPECT_EQ(chen::ip::address_v4(""), chen::ip::address_v4("0.0.0.0"));

    EXPECT_EQ(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1/8"));
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1", 8));
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1", "255.0.0.0"));

    EXPECT_EQ(chen::ip::address_v4("127"), chen::ip::address_v4("127.0.0.0"));
    EXPECT_EQ(chen::ip::address_v4("127.1"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_EQ(chen::ip::address_v4("192.168.1"), chen::ip::address_v4("192.168.0.1"));

    EXPECT_EQ(chen::ip::address_v4(0x7F000001), chen::ip::address_v4("127.0.0.1"));
    EXPECT_EQ(chen::ip::address_v4(0x7F000001, 8), chen::ip::address_v4("127.0.0.1/8"));
    EXPECT_EQ(chen::ip::address_v4(0x7F000001, "255.0.0.0"), chen::ip::address_v4("127.0.0.1/8"));

    EXPECT_THROW(chen::ip::address_v4("127.0.0.1", 33), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v4(0x7F000001, 33), chen::ip::address::error);

    // representation
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.0.1").str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.1").str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4(0x7F000001).str());
    EXPECT_EQ("127.0.0.1/32", chen::ip::address_v4("127.0.0.1").full());
    EXPECT_EQ("127.0.0.1/8", chen::ip::address_v4("127.0.0.1/8").full());
    EXPECT_EQ("127.0.0.1/8", chen::ip::address_v4(0x7F000001, 8).full());
    EXPECT_EQ("192.168.1.1/30", chen::ip::address_v4("192.168.1.1", 30).full());

    // network
    EXPECT_EQ(8, chen::ip::address_v4("127.0.0.1/8").cidr());
    EXPECT_EQ(16, chen::ip::address_v4("180.166.92.36/16").cidr());
    EXPECT_EQ(24, chen::ip::address_v4("192.168.0.1/24").cidr());
    EXPECT_EQ(26, chen::ip::address_v4("192.168.0.1/26").cidr());

    EXPECT_EQ(chen::ip::address_v4("255.0.0.0").addr(), chen::ip::address_v4("127.0.0.1/8").netmask());
    EXPECT_EQ(chen::ip::address_v4("0.255.255.255").addr(), chen::ip::address_v4("127.0.0.1/8").wildcard());

    EXPECT_EQ("127.0.0.0/8", chen::ip::address_v4("127.0.0.1/8").network().full());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.0.0.1/8").minhost().str());
    EXPECT_EQ("127.255.255.254", chen::ip::address_v4("127.0.0.1/8").maxhost().str());
    EXPECT_EQ("127.255.255.255", chen::ip::address_v4("127.0.0.1/8").broadcast().str());
    EXPECT_EQ(16777214, chen::ip::address_v4("127.0.0.1/8").hosts());

    // special
    EXPECT_TRUE(chen::ip::address_v4("0.0.0.0/8").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("127.0.0.1/8").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("169.254.0.0/16").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("192.0.0.0/24").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("192.0.2.0/24").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("192.88.99.0/24").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("198.51.100.0/24").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("203.0.113.0/24").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("224.0.0.0/32").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("240.0.0.0/32").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("10.0.0.0/8").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("100.64.0.0/10").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("172.16.0.0/12").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("192.168.0.0/16").isReserved());
    EXPECT_TRUE(chen::ip::address_v4("198.18.0.0/15").isReserved());

    EXPECT_TRUE(chen::ip::address_v4("10.0.0.0/8").isPrivate());
    EXPECT_TRUE(chen::ip::address_v4("100.64.0.0/10").isPrivate());
    EXPECT_TRUE(chen::ip::address_v4("172.16.0.0/12").isPrivate());
    EXPECT_TRUE(chen::ip::address_v4("192.168.0.0/16").isPrivate());
    EXPECT_TRUE(chen::ip::address_v4("198.18.0.0/15").isPrivate());

    EXPECT_TRUE(chen::ip::address_v4("127.0.0.1/8").isLoopback());
    EXPECT_FALSE(chen::ip::address_v4("192.168.0.1/24").isLoopback());

    EXPECT_TRUE(chen::ip::address_v4("169.254.0.0/16").isLinkLocal());

    EXPECT_TRUE(chen::ip::address_v4("224.0.0.115/32").isMulticast());
    EXPECT_FALSE(chen::ip::address_v4("10.0.0.1/8").isMulticast());

    EXPECT_TRUE(chen::ip::address_v4("192.168.0.255/24").isBroadcast());
    EXPECT_FALSE(chen::ip::address_v4("192.168.0.1/24").isBroadcast());

    // class A
    EXPECT_TRUE(chen::ip::address_v4("0.0.0.0").isClassA());
    EXPECT_FALSE(chen::ip::address_v4("0.0.0.0").isClassB());
    EXPECT_FALSE(chen::ip::address_v4("0.0.0.0").isClassC());

    EXPECT_TRUE(chen::ip::address_v4("10.0.0.0").isClassA());
    EXPECT_FALSE(chen::ip::address_v4("10.0.0.0").isClassB());
    EXPECT_FALSE(chen::ip::address_v4("10.0.0.0").isClassC());

    EXPECT_TRUE(chen::ip::address_v4("127.255.255.255").isClassA());
    EXPECT_FALSE(chen::ip::address_v4("127.255.255.255").isClassB());
    EXPECT_FALSE(chen::ip::address_v4("127.255.255.255").isClassC());

    // class B
    EXPECT_FALSE(chen::ip::address_v4("128.0.0.0").isClassA());
    EXPECT_TRUE(chen::ip::address_v4("128.0.0.0").isClassB());
    EXPECT_FALSE(chen::ip::address_v4("128.0.0.0").isClassC());

    EXPECT_FALSE(chen::ip::address_v4("190.0.0.0").isClassA());
    EXPECT_TRUE(chen::ip::address_v4("190.0.0.0").isClassB());
    EXPECT_FALSE(chen::ip::address_v4("190.0.0.0").isClassC());

    EXPECT_FALSE(chen::ip::address_v4("191.255.255.255").isClassA());
    EXPECT_TRUE(chen::ip::address_v4("191.255.255.255").isClassB());
    EXPECT_FALSE(chen::ip::address_v4("191.255.255.255").isClassC());

    // class C
    EXPECT_FALSE(chen::ip::address_v4("192.0.0.0").isClassA());
    EXPECT_FALSE(chen::ip::address_v4("192.0.0.0").isClassB());
    EXPECT_TRUE(chen::ip::address_v4("192.0.0.0").isClassC());

    EXPECT_FALSE(chen::ip::address_v4("192.168.1.1").isClassA());
    EXPECT_FALSE(chen::ip::address_v4("192.168.1.1").isClassB());
    EXPECT_TRUE(chen::ip::address_v4("192.168.1.1").isClassC());

    EXPECT_FALSE(chen::ip::address_v4("223.255.255.255").isClassA());
    EXPECT_FALSE(chen::ip::address_v4("223.255.255.255").isClassB());
    EXPECT_TRUE(chen::ip::address_v4("223.255.255.255").isClassC());

    // class D & E
    EXPECT_TRUE(chen::ip::address_v4("224.0.0.0").isClassD());
    EXPECT_TRUE(chen::ip::address_v4("239.255.255.255").isClassD());

    EXPECT_TRUE(chen::ip::address_v4("240.0.0.0").isClassE());
    EXPECT_TRUE(chen::ip::address_v4("255.255.255.255").isClassE());

    // operator
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1/8"));

    EXPECT_NE(chen::ip::address_v4("127.0.0.0"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_NE(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1"));

    EXPECT_LT(chen::ip::address_v4("127.0.0.0"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_LT(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1"));

    EXPECT_GT(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.0"));
    EXPECT_GT(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.1/8"));

    EXPECT_LE(chen::ip::address_v4("127.0.0.0"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_LE(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_LE(chen::ip::address_v4("127.0.0.1/8"), chen::ip::address_v4("127.0.0.1"));

    EXPECT_GE(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.0"));
    EXPECT_GE(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.1"));
    EXPECT_GE(chen::ip::address_v4("127.0.0.1"), chen::ip::address_v4("127.0.0.1/8"));

    // invalid test
    EXPECT_THROW(chen::ip::address_v4::toInteger("127..1"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v4::toInteger("999.0.0.0"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v4::toInteger("127.0.0.1/99"), chen::ip::address::error);
}