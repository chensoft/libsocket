/**
 * Created by Jian Chen
 * @since  2016.07.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/so/so_endpoint.hpp>
#include <gtest/gtest.h>

TEST(SOEndpointTest, General)
{
    const chen::so::endpoint ep1("127.0.0.1", 80);
    const chen::so::endpoint ep2(chen::ip::address_v4("127.0.0.1"), 80);

    EXPECT_EQ(80, ep1.port());

    EXPECT_EQ(chen::ip::address_v4("127.0.0.1"), ep1.addr());
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1"), ep2.addr());

    chen::so::endpoint ep3("127.0.0.1", 80);

    EXPECT_EQ(80, ep3.port());
    EXPECT_EQ(chen::ip::address_v4("127.0.0.1"), ep3.addr());

    ep3.port(443);
    ep3.addr(chen::ip::address_v4("192.168.1.1"));

    EXPECT_EQ(443, ep3.port());
    EXPECT_EQ(chen::ip::address_v4("192.168.1.1").str(), ep3.addr().str());

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