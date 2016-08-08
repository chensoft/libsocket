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

TEST(IPSubnetTest, IPv6)
{
    // assign
    EXPECT_EQ(chen::ip::subnet_v6(), chen::ip::subnet_v6("::"));
    EXPECT_EQ(chen::ip::subnet_v6("::1/64"), chen::ip::subnet_v6("::1/64"));
    EXPECT_EQ(chen::ip::subnet_v6("::1/64"), chen::ip::subnet_v6("::1", 64));

    EXPECT_EQ(chen::ip::subnet_v6("::"), chen::ip::subnet_v6("0000:0000:0000:0000:0000:0000:0000:0000"));
    EXPECT_EQ(chen::ip::subnet_v6("::"), chen::ip::subnet_v6("0:0:0:0:0:0:0:0"));

    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817:0:0:0:200e"));
    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817:0000:0000:0000:200e"));
    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::200e/128"));

    EXPECT_EQ(chen::ip::subnet_v6("::c0a8:1"), chen::ip::subnet_v6("::192.168.0.1"));

    std::array<std::uint8_t, 16> bytes = {{0x24, 0x04, 0x68, 0, 0x40, 0x04, 0x08, 0x17, 0, 0, 0, 0, 0, 0, 0x20, 0x0e}};

    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6(bytes));
    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e/64"), chen::ip::subnet_v6(bytes, 64));

    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6(std::array<std::uint8_t, 16>(bytes)));
    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e/64"), chen::ip::subnet_v6(std::array<std::uint8_t, 16>(bytes), 64));

    EXPECT_THROW(chen::ip::subnet_v6("2404:6800:4004:817::200e/129"), chen::ip::error_subnet);
    EXPECT_THROW(chen::ip::subnet_v6(bytes, 129), chen::ip::error_subnet);

    chen::ip::subnet_v6 v6("::1");
    EXPECT_EQ("::1/128", v6.str());

    v6 = "2404:6800:4004:817::200e";
    EXPECT_EQ("2404:6800:4004:817::200e/128", v6.str());
    EXPECT_EQ(128, v6.cidr());

    v6 = bytes;
    EXPECT_EQ("2404:6800:4004:817::200e/128", v6.str());
    EXPECT_EQ(128, v6.cidr());

    // representation
    EXPECT_EQ("::/128", chen::ip::subnet_v6().str());
    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::subnet_v6("2404:6800:4004:817:0000:0000:0000:200e").str());

    EXPECT_EQ(bytes, chen::ip::subnet_v6("2404:6800:4004:817::200e").addr());
    EXPECT_EQ(128, chen::ip::subnet_v6("2404:6800:4004:817::200e").cidr());
    EXPECT_EQ(128, chen::ip::subnet_v6("2404:6800:4004:817::200e/128").cidr());
    EXPECT_EQ(64, chen::ip::subnet_v6("2404:6800:4004:817::200e/64").cidr());

    // network
    std::array<std::uint8_t, 16> netmask = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    std::array<std::uint8_t, 16> wildcard = {{0, 0, 0, 0, 0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, chen::ip::subnet_v6("2404:6800:4004:817::200e/48").netmask());
    EXPECT_EQ(wildcard, chen::ip::subnet_v6("2404:6800:4004:817::200e/48").wildcard());

    netmask  = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    wildcard = {{0, 0, 0, 0, 0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, chen::ip::subnet_v6("2404:6800:4004:817::200e/46").netmask());
    EXPECT_EQ(wildcard, chen::ip::subnet_v6("2404:6800:4004:817::200e/46").wildcard());

    EXPECT_EQ("2404:6800:4004::/46", chen::ip::subnet_v6("2404:6800:4004:817::200e/46").network().str());
    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::subnet_v6("2404:6800:4004:817::200e/128").network().str());

    EXPECT_EQ("2404:6800:4004::/46", chen::ip::subnet_v6("2404:6800:4004:817::200e/46").minhost().str());
    EXPECT_EQ("2404:6800:4007:ffff:ffff:ffff:ffff:ffff/46", chen::ip::subnet_v6("2404:6800:4004:817::200e/46").maxhost().str());

    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::subnet_v6("2404:6800:4004:817::200e/128").minhost().str());
    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::subnet_v6("2404:6800:4004:817::200e/128").maxhost().str());

    // operator
    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));
    EXPECT_EQ(chen::ip::subnet_v6("2404:6800:4004:817::200e/64"), chen::ip::subnet_v6("2404:6800:4004:817::200e/64"));

    EXPECT_NE(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::"));
    EXPECT_NE(chen::ip::subnet_v6("2404:6800:4004:817::200e/64"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));

    EXPECT_LT(chen::ip::subnet_v6("2404:6800:4004:817::"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));
    EXPECT_LT(chen::ip::subnet_v6("2404:6800:4004:817::200e/64"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));

    EXPECT_GT(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::"));
    EXPECT_GT(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::200e/64"));

    EXPECT_LE(chen::ip::subnet_v6("2404:6800:4004:817::"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));
    EXPECT_LE(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));
    EXPECT_LE(chen::ip::subnet_v6("2404:6800:4004:817::200e/64"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));

    EXPECT_GE(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::"));
    EXPECT_GE(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::200e"));
    EXPECT_GE(chen::ip::subnet_v6("2404:6800:4004:817::200e"), chen::ip::subnet_v6("2404:6800:4004:817::200e/64"));

    // invalid test
    EXPECT_THROW(chen::ip::subnet_v6("2404:6800:4004:817::200e", 200), chen::ip::error_subnet);
    EXPECT_THROW(chen::ip::subnet_v6(bytes, 200), chen::ip::error_subnet);
}