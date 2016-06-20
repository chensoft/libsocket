/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/socket.hpp>
#include <gtest/gtest.h>

TEST(NetIPAddrTest, Assign)
{
    chen::ip::address v4;

    // empty: 0.0.0.0
    v4.assign("");
    EXPECT_EQ("0.0.0.0", v4.str());

    // one: 127 -> 127.0.0.0
    v4.assign("127");
    EXPECT_EQ("127.0.0.0", v4.str());

    // two: 127.1 -> 127.0.0.1
    v4.assign("127.1");
    EXPECT_EQ("127.0.0.1", v4.str());

    // three: 1.2.3 -> 1.2.0.3
    v4.assign("1.2.3");
    EXPECT_EQ("1.2.0.3", v4.str());

    // four: 192.168.10.1
    v4.assign("192.168.10.1");
    EXPECT_EQ("192.168.10.1", v4.str());

    // full representation
    v4.assign("192.168.1.1", 30);
    EXPECT_EQ("192.168.1.1/30", v4.full());
}

TEST(NetIPAddrTest, Class)
{
    chen::ip::address v4;

    // class A
    v4.assign("0.0.0.0");
    EXPECT_TRUE(v4.isClassA());
    EXPECT_FALSE(v4.isClassB());
    EXPECT_FALSE(v4.isClassC());

    v4.assign("10.0.0.0");
    EXPECT_TRUE(v4.isClassA());
    EXPECT_FALSE(v4.isClassB());
    EXPECT_FALSE(v4.isClassC());

    v4.assign("127.255.255.255");
    EXPECT_TRUE(v4.isClassA());
    EXPECT_FALSE(v4.isClassB());
    EXPECT_FALSE(v4.isClassC());

    // class B
    v4.assign("128.0.0.0");
    EXPECT_FALSE(v4.isClassA());
    EXPECT_TRUE(v4.isClassB());
    EXPECT_FALSE(v4.isClassC());

    v4.assign("190.0.0.0");
    EXPECT_FALSE(v4.isClassA());
    EXPECT_TRUE(v4.isClassB());
    EXPECT_FALSE(v4.isClassC());

    v4.assign("191.255.255.255");
    EXPECT_FALSE(v4.isClassA());
    EXPECT_TRUE(v4.isClassB());
    EXPECT_FALSE(v4.isClassC());

    // class C
    v4.assign("192.0.0.0");
    EXPECT_FALSE(v4.isClassA());
    EXPECT_FALSE(v4.isClassB());
    EXPECT_TRUE(v4.isClassC());

    v4.assign("192.168.1.1");
    EXPECT_FALSE(v4.isClassA());
    EXPECT_FALSE(v4.isClassB());
    EXPECT_TRUE(v4.isClassC());

    v4.assign("223.255.255.255");
    EXPECT_FALSE(v4.isClassA());
    EXPECT_FALSE(v4.isClassB());
    EXPECT_TRUE(v4.isClassC());
}

TEST(NetIPAddrTest, Property)
{
    chen::ip::address v4;

    // empty
    v4.assign("");
    EXPECT_TRUE(v4.empty());

    // loopback
    v4.assign("127.0.0.1");
    EXPECT_TRUE(v4.isLoopback());

    v4.assign("192.168.0.1");
    EXPECT_FALSE(v4.isLoopback());

    // broadcast
    v4.assign("192.168.0.255", 24);
    EXPECT_TRUE(v4.isBroadcast());

    v4.assign("192.168.0.1", 24);
    EXPECT_FALSE(v4.isBroadcast());

    // multicast
    v4.assign("224.0.0.115");
    EXPECT_TRUE(v4.isMulticast());

    v4.assign("10.0.0.1");
    EXPECT_FALSE(v4.isMulticast());

    // network
    v4.assign("192.168.1.10");

    EXPECT_EQ("192.168.1.0", v4.network().str());
    EXPECT_EQ("192.168.1.1", v4.hostMin().str());
    EXPECT_EQ("192.168.1.254", v4.hostMax().str());

    // broadcast
    v4.assign("192.168.1.1", 30);
    EXPECT_EQ("192.168.1.3", v4.broadcast().str());
}

TEST(NetIPAddrTest, Operator)
{
    chen::ip::address a, b;

    // operator
    a.assign("127.0.0.1");
    b.assign("127.0.0.1");

    EXPECT_EQ(a, b);

    a.assign("127.0.0.0");
    b.assign("127.0.0.1");

    EXPECT_NE(a, b);

    EXPECT_LT(a, b);
    EXPECT_GT(b, a);

    EXPECT_LE(a, b);
    EXPECT_GE(b, a);
}