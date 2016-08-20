/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_endpoint.hpp>
#include <gtest/gtest.h>

TEST(NetEndpointTest, General)
{
    using chen::net::address;
    using chen::net::endpoint;

    EXPECT_TRUE(endpoint(nullptr).empty());

    const endpoint ep1("127.0.0.1", 80);
    const endpoint ep2(address("127.0.0.1"), 80);

    EXPECT_EQ(80, ep1.port());

    EXPECT_EQ("127.0.0.1", ep1.addr().str());
    EXPECT_EQ("127.0.0.1", ep2.addr().str());

    endpoint ep3("127.0.0.1", 80);

    EXPECT_EQ(80, ep3.port());
    EXPECT_EQ(address("127.0.0.1"), ep3.addr());

    ep3.port(443);
    ep3.addr(address("192.168.1.1"));

    EXPECT_EQ(443, ep3.port());
    EXPECT_EQ(address("192.168.1.1").str(), ep3.addr().str());

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