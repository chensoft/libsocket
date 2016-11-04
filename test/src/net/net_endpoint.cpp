/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/prot/inet/net_endpoint.hpp>
#include <gtest/gtest.h>

TEST(NetEndpointTest, General)
{
    using chen::net::endpoint;

    // construct
    EXPECT_TRUE(endpoint(nullptr).empty());
    EXPECT_FALSE(endpoint(nullptr));

    EXPECT_EQ("0.0.0.0:80", endpoint(":80").str());
    EXPECT_EQ("127.0.0.1:80", endpoint("127.0.0.1", 80).str());
    EXPECT_EQ("127.0.0.1:0", endpoint("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1:80", endpoint("127.0.0.1:80").str());
    EXPECT_EQ("127.0.0.1:80", endpoint("127.0.0.1:http").str());

    EXPECT_EQ("[::]:80", endpoint("[::]:80").str());
    EXPECT_EQ("[fe80::1]:0", endpoint("[fe80::1]").str());
    EXPECT_EQ("[fe80::1]:80", endpoint("[fe80::1]:80").str());
    EXPECT_EQ("[fe80::1]:80", endpoint("[fe80::1]:http").str());
}

TEST(NetEndpointTest, Property)
{
    using chen::net::endpoint;
    using chen::ip::address;

    // modify port and address
    endpoint ep("127.0.0.1", 80);

    EXPECT_EQ(80, ep.port());
    EXPECT_EQ("127.0.0.1", ep.addr().str());

    ep.port(443);
    ep.addr(address("192.168.1.1"));

    EXPECT_EQ(443, ep.port());
    EXPECT_EQ("192.168.1.1", ep.addr().str());

    // address with port
    EXPECT_EQ("127.0.0.1:80", endpoint("127.0.0.1", 80).str());
    EXPECT_EQ("[::1]:80", endpoint("::1", 80).str());
}

TEST(NetEndpointTest, Special)
{
    using chen::net::endpoint;

    // special ports
    EXPECT_TRUE(endpoint("127.0.0.1", 0).isWellKnownPort());
    EXPECT_FALSE(endpoint("127.0.0.1", 0).isRegisteredPort());
    EXPECT_FALSE(endpoint("127.0.0.1", 0).isDynamicPort());

    EXPECT_TRUE(endpoint("127.0.0.1", 80).isWellKnownPort());
    EXPECT_TRUE(endpoint("127.0.0.1", 1023).isWellKnownPort());

    EXPECT_TRUE(endpoint("127.0.0.1", 1024).isRegisteredPort());
    EXPECT_TRUE(endpoint("127.0.0.1", 3306).isRegisteredPort());
    EXPECT_TRUE(endpoint("127.0.0.1", 49151).isRegisteredPort());

    EXPECT_TRUE(endpoint("127.0.0.1", 49152).isDynamicPort());
    EXPECT_TRUE(endpoint("127.0.0.1", 50000).isDynamicPort());
    EXPECT_TRUE(endpoint("127.0.0.1", 65535).isDynamicPort());
}

TEST(NetEndpointTest, Compare)
{
    using chen::net::endpoint;

    // compare
    EXPECT_EQ(endpoint("127.0.0.1", 80), endpoint("127.0.0.1", 80));
    EXPECT_NE(endpoint("192.168.0.1", 80), endpoint("127.0.0.1", 80));

    EXPECT_LT(endpoint("127.0.0.0", 80), endpoint("127.0.0.1", 80));
    EXPECT_LT(endpoint("127.0.0.1", 53), endpoint("127.0.0.1", 80));

    EXPECT_LE(endpoint("127.0.0.1", 80), endpoint("127.0.0.1", 80));
    EXPECT_LE(endpoint("127.0.0.1", 53), endpoint("127.0.0.1", 80));

    EXPECT_GT(endpoint("127.0.0.1", 80), endpoint("127.0.0.0", 80));
    EXPECT_GT(endpoint("127.0.0.1", 80), endpoint("127.0.0.1", 53));

    EXPECT_GE(endpoint("127.0.0.1", 80), endpoint("127.0.0.1", 80));
    EXPECT_GE(endpoint("127.0.0.1", 80), endpoint("127.0.0.1", 53));
}