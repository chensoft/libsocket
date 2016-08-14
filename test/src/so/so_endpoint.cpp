/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/so/so_endpoint.hpp>
#include <gtest/gtest.h>

using namespace chen::ip;
using namespace chen::so;

TEST(SOEndpointTest, General)
{
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