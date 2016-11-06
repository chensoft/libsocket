/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_address.hpp>
#include <gtest/gtest.h>

TEST(InetAddressTest, General)
{
    using chen::inet_address;

    // construct
    EXPECT_TRUE(inet_address(nullptr).empty());
    EXPECT_FALSE(inet_address(nullptr));

    EXPECT_EQ("0.0.0.0:80", inet_address(":80").str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1", 80).str());
    EXPECT_EQ("127.0.0.1:0", inet_address("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1:80").str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1:http").str());

    EXPECT_EQ("[::]:80", inet_address("[::]:80").str());
    EXPECT_EQ("[fe80::1]:0", inet_address("[fe80::1]").str());
    EXPECT_EQ("[fe80::1]:80", inet_address("[fe80::1]:80").str());
    EXPECT_EQ("[fe80::1]:80", inet_address("[fe80::1]:http").str());
}

TEST(InetAddressTest, Property)
{
    using chen::inet_address;
    using chen::ip_address;

    // modify port and address
    inet_address addr("127.0.0.1", 80);

    EXPECT_EQ(80, addr.port());
    EXPECT_EQ("127.0.0.1", addr.addr().str());

    addr.port(443);
    addr.addr(ip_address("192.168.1.1"));

    EXPECT_EQ(443, addr.port());
    EXPECT_EQ("192.168.1.1", addr.addr().str());

    // address with port
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1", 80).str());
    EXPECT_EQ("[::1]:80", inet_address("::1", 80).str());
}

TEST(InetAddressTest, Special)
{
    using chen::inet_address;

    // special ports
    EXPECT_TRUE(inet_address("127.0.0.1", 0).isWellKnownPort());
    EXPECT_FALSE(inet_address("127.0.0.1", 0).isRegisteredPort());
    EXPECT_FALSE(inet_address("127.0.0.1", 0).isDynamicPort());

    EXPECT_TRUE(inet_address("127.0.0.1", 80).isWellKnownPort());
    EXPECT_TRUE(inet_address("127.0.0.1", 1023).isWellKnownPort());

    EXPECT_TRUE(inet_address("127.0.0.1", 1024).isRegisteredPort());
    EXPECT_TRUE(inet_address("127.0.0.1", 3306).isRegisteredPort());
    EXPECT_TRUE(inet_address("127.0.0.1", 49151).isRegisteredPort());

    EXPECT_TRUE(inet_address("127.0.0.1", 49152).isDynamicPort());
    EXPECT_TRUE(inet_address("127.0.0.1", 50000).isDynamicPort());
    EXPECT_TRUE(inet_address("127.0.0.1", 65535).isDynamicPort());
}

TEST(InetAddressTest, Compare)
{
    using chen::inet_address;

    // compare
    EXPECT_EQ(inet_address("127.0.0.1", 80), inet_address("127.0.0.1", 80));
    EXPECT_NE(inet_address("192.168.0.1", 80), inet_address("127.0.0.1", 80));

    EXPECT_LT(inet_address("127.0.0.0", 80), inet_address("127.0.0.1", 80));
    EXPECT_LT(inet_address("127.0.0.1", 53), inet_address("127.0.0.1", 80));

    EXPECT_LE(inet_address("127.0.0.1", 80), inet_address("127.0.0.1", 80));
    EXPECT_LE(inet_address("127.0.0.1", 53), inet_address("127.0.0.1", 80));

    EXPECT_GT(inet_address("127.0.0.1", 80), inet_address("127.0.0.0", 80));
    EXPECT_GT(inet_address("127.0.0.1", 80), inet_address("127.0.0.1", 53));

    EXPECT_GE(inet_address("127.0.0.1", 80), inet_address("127.0.0.1", 80));
    EXPECT_GE(inet_address("127.0.0.1", 80), inet_address("127.0.0.1", 53));
}