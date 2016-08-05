/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_address.hpp>
#include <gtest/gtest.h>

TEST(IPAddressTest, Base)
{
    auto ptr = chen::ip::address::create("127.0.0.1");

    EXPECT_TRUE(chen::ip::address::isIPv4("127.0.0.1"));
    EXPECT_FALSE(chen::ip::address::isIPv6("127.0.0.1"));

    EXPECT_NE(nullptr, std::dynamic_pointer_cast<chen::ip::address_v4>(ptr));
    EXPECT_EQ(nullptr, std::dynamic_pointer_cast<chen::ip::address_v6>(ptr));
    EXPECT_EQ("127.0.0.1", ptr->str());
    EXPECT_EQ(ptr.get(), ptr->v4());
    EXPECT_EQ(nullptr, ptr->v6());

    ptr = chen::ip::address::create("127.0.0.1", 8);

    EXPECT_NE(nullptr, std::dynamic_pointer_cast<chen::ip::address_v4>(ptr));
    EXPECT_EQ(nullptr, std::dynamic_pointer_cast<chen::ip::address_v6>(ptr));
    EXPECT_EQ("127.0.0.1", ptr->str());
    EXPECT_EQ(ptr.get(), ptr->v4());
    EXPECT_EQ(nullptr, ptr->v6());

    ptr = chen::ip::address::create("2404:6800:4004:817::200e");

    EXPECT_TRUE(chen::ip::address::isIPv6("2404:6800:4004:817::200e"));
    EXPECT_FALSE(chen::ip::address::isIPv4("2404:6800:4004:817::200e"));

    EXPECT_NE(nullptr, std::dynamic_pointer_cast<chen::ip::address_v6>(ptr));
    EXPECT_EQ(nullptr, std::dynamic_pointer_cast<chen::ip::address_v4>(ptr));
    EXPECT_EQ("2404:6800:4004:817::200e", ptr->str());
    EXPECT_EQ(nullptr, ptr->v4());
    EXPECT_EQ(ptr.get(), ptr->v6());

    ptr = chen::ip::address::create("2404:6800:4004:817::200e", 64);

    EXPECT_NE(nullptr, std::dynamic_pointer_cast<chen::ip::address_v6>(ptr));
    EXPECT_EQ(nullptr, std::dynamic_pointer_cast<chen::ip::address_v4>(ptr));
    EXPECT_EQ("2404:6800:4004:817::200e", ptr->str());
    EXPECT_EQ(nullptr, ptr->v4());
    EXPECT_EQ(ptr.get(), ptr->v6());

    std::shared_ptr<const chen::ip::address> p = chen::ip::address::create("2404:6800:4004:817::200e", 64);

    EXPECT_EQ(nullptr, p->v4());
    EXPECT_EQ(p.get(), p->v6());
}

TEST(IPAddressTest, IPv4)
{
    // assign
    EXPECT_EQ(chen::ip::address_v4(), chen::ip::address_v4("0.0.0.0"));

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

    chen::ip::address_v4 v4("127");
    EXPECT_EQ("127.0.0.0", v4.str());

    v4 = "192.168.1.1";
    EXPECT_EQ("192.168.1.1", v4.str());

    // representation
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.0.1").str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4("127.1").str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4(0x7F000001).str());
    EXPECT_EQ("127.0.0.1/32", chen::ip::address_v4("127.0.0.1").notation());
    EXPECT_EQ("127.0.0.1/8", chen::ip::address_v4("127.0.0.1/8").notation());
    EXPECT_EQ("127.0.0.1/8", chen::ip::address_v4(0x7F000001, 8).notation());
    EXPECT_EQ("192.168.1.1/30", chen::ip::address_v4("192.168.1.1", 30).notation());

    // network
    EXPECT_EQ(8, chen::ip::address_v4("127.0.0.1/8").cidr());
    EXPECT_EQ(16, chen::ip::address_v4("180.166.92.36/16").cidr());
    EXPECT_EQ(24, chen::ip::address_v4("192.168.0.1/24").cidr());
    EXPECT_EQ(26, chen::ip::address_v4("192.168.0.1/26").cidr());

    EXPECT_EQ(chen::ip::address_v4("255.0.0.0").addr(), chen::ip::address_v4("127.0.0.1/8").netmask());
    EXPECT_EQ(chen::ip::address_v4("0.255.255.255").addr(), chen::ip::address_v4("127.0.0.1/8").wildcard());

    EXPECT_EQ("127.0.0.0/8", chen::ip::address_v4("127.0.0.1/8").network().notation());
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

    // common
    EXPECT_EQ("0.0.0.0", chen::ip::address_v4::any().str());
    EXPECT_EQ("255.255.255.255", chen::ip::address_v4::broad().str());
    EXPECT_EQ("127.0.0.1", chen::ip::address_v4::loopback().str());

    // invalid test
    EXPECT_THROW(chen::ip::address_v4::toInteger("127..1"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v4::toInteger("999.0.0.0"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v4::toInteger("127.0.0.1/99"), chen::ip::address::error);
}

TEST(IPAddressTest, IPv6)
{
    // assign
    EXPECT_EQ(chen::ip::address_v6(), chen::ip::address_v6("::"));
    EXPECT_EQ(chen::ip::address_v6("::1/64"), chen::ip::address_v6("::1/64"));
    EXPECT_EQ(chen::ip::address_v6("::1/64"), chen::ip::address_v6("::1", 64));

    EXPECT_EQ(chen::ip::address_v6("::"), chen::ip::address_v6("0000:0000:0000:0000:0000:0000:0000:0000"));
    EXPECT_EQ(chen::ip::address_v6("::"), chen::ip::address_v6("0:0:0:0:0:0:0:0"));

    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817:0:0:0:200e"));
    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:200e"));
    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::200e/128"));

    EXPECT_EQ(chen::ip::address_v6("::c0a8:1"), chen::ip::address_v6("::192.168.0.1"));

    std::array<std::uint8_t, 16> bytes = {{0x24, 0x04, 0x68, 0, 0x40, 0x04, 0x08, 0x17, 0, 0, 0, 0, 0, 0, 0x20, 0x0e}};

    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6(bytes));
    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e/64"), chen::ip::address_v6(bytes, 64));

    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6(std::array<std::uint8_t, 16>(bytes)));
    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e/64"), chen::ip::address_v6(std::array<std::uint8_t, 16>(bytes), 64));

    EXPECT_THROW(chen::ip::address_v6("2404:6800:4004:817::200e/129"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v6(bytes, 129), chen::ip::address::error);

    chen::ip::address_v6 v6("::1");
    EXPECT_EQ("::1", v6.str());

    v6 = "2404:6800:4004:817::200e";
    EXPECT_EQ("2404:6800:4004:817::200e", v6.str());

    // representation
    EXPECT_EQ("::", chen::ip::address_v6().str());
    EXPECT_EQ("2404:6800:4004:817::", chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:0000").str());
    EXPECT_EQ("2404::817:0:0:0:200e", chen::ip::address_v6("2404:0000:0000:817:0000:0000:0000:200e").str());
    EXPECT_EQ("2404:6800:4004:817::200e", chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:200e").str());
    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:200e").notation());
    EXPECT_EQ("2404:6800:4004:0817:0000:0000:0000:200e", chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:200e").expanded());
    EXPECT_EQ("2404:6800:4004:817:0:0:0:200e", chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:200e").suppressed());
    EXPECT_EQ("2404:6800:4004:817::200e", chen::ip::address_v6("2404:6800:4004:817:0000:0000:0000:200e").compressed());
    EXPECT_EQ("::ffff:192.0.2.128", chen::ip::address_v6("::ffff:c000:280").mixed());

    EXPECT_EQ(chen::ip::address_v4("192.0.2.128"), chen::ip::address_v6("::ffff:c000:280").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("2001:db8:c000:221::/32").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("2001:db8:1c0:2:21::/40").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("2001:db8:122:c000:2:2100::/48").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("2001:db8:122:3c0:0:221::/56").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("2001:db8:122:344:c0:2:2100::/64").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("2001:db8:122:344::192.0.2.33/96").embedded());
    EXPECT_EQ(chen::ip::address_v4("192.0.2.33"), chen::ip::address_v6("64:ff9b::192.0.2.33/96").embedded());

    EXPECT_EQ(bytes, chen::ip::address_v6("2404:6800:4004:817::200e").addr());
    EXPECT_EQ(128, chen::ip::address_v6("2404:6800:4004:817::200e").cidr());
    EXPECT_EQ(128, chen::ip::address_v6("2404:6800:4004:817::200e/128").cidr());
    EXPECT_EQ(64, chen::ip::address_v6("2404:6800:4004:817::200e/64").cidr());

    // network
    std::array<std::uint8_t, 16> netmask = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    std::array<std::uint8_t, 16> wildcard = {{0, 0, 0, 0, 0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, chen::ip::address_v6("2404:6800:4004:817::200e/48").netmask());
    EXPECT_EQ(wildcard, chen::ip::address_v6("2404:6800:4004:817::200e/48").wildcard());

    netmask  = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    wildcard = {{0, 0, 0, 0, 0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, chen::ip::address_v6("2404:6800:4004:817::200e/46").netmask());
    EXPECT_EQ(wildcard, chen::ip::address_v6("2404:6800:4004:817::200e/46").wildcard());

    EXPECT_EQ("2404:6800:4004::/46", chen::ip::address_v6("2404:6800:4004:817::200e/46").network().notation());
    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::address_v6("2404:6800:4004:817::200e/128").network().notation());

    EXPECT_EQ("2404:6800:4004::/46", chen::ip::address_v6("2404:6800:4004:817::200e/46").minhost().notation());
    EXPECT_EQ("2404:6800:4007:ffff:ffff:ffff:ffff:ffff/46", chen::ip::address_v6("2404:6800:4004:817::200e/46").maxhost().notation());

    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::address_v6("2404:6800:4004:817::200e/128").minhost().notation());
    EXPECT_EQ("2404:6800:4004:817::200e/128", chen::ip::address_v6("2404:6800:4004:817::200e/128").maxhost().notation());

    // special
    EXPECT_TRUE(chen::ip::address_v6("::").isUnspecified());
    EXPECT_FALSE(chen::ip::address_v6("::1").isUnspecified());

    EXPECT_TRUE(chen::ip::address_v6("::1").isLoopback());
    EXPECT_FALSE(chen::ip::address_v6("::").isLoopback());

    EXPECT_TRUE(chen::ip::address_v6("2404:6800:4004:817::200e").isGlobalUnicast());
    EXPECT_FALSE(chen::ip::address_v6("fe80::7a31:c1ff:fec2:b5aa").isGlobalUnicast());

    EXPECT_TRUE(chen::ip::address_v6("fe80::7a31:c1ff:fec2:b5aa").isLinkLocalUnicast());
    EXPECT_FALSE(chen::ip::address_v6("2404:6800:4004:817::200e").isLinkLocalUnicast());

    EXPECT_TRUE(chen::ip::address_v6("FEC0::1234:5678:9ABC").isSiteLocalUnicast());
    EXPECT_FALSE(chen::ip::address_v6("2404:6800:4004:817::200e").isSiteLocalUnicast());

    EXPECT_TRUE(chen::ip::address_v6("::192.168.1.1").isIPv4Compatible());
    EXPECT_FALSE(chen::ip::address_v6("2404::192.168.1.1").isIPv4Compatible());

    EXPECT_TRUE(chen::ip::address_v6("::ffff:192.168.1.1").isIPv4Mapped());
    EXPECT_FALSE(chen::ip::address_v6("::192.168.1.1").isIPv4Mapped());

    EXPECT_TRUE(chen::ip::address_v6("::ffff:192.168.1.1").isIPv4Mapped());
    EXPECT_FALSE(chen::ip::address_v6("::192.168.1.1").isIPv4Mapped());

    EXPECT_TRUE(chen::ip::address_v6("ff02::1").isMulticast());
    EXPECT_FALSE(chen::ip::address_v6("2404::192.168.1.1").isMulticast());

    EXPECT_TRUE(chen::ip::address_v6("64:ff9b::73b6:405e/96").isIPv4EmbeddedWellKnown());
    EXPECT_FALSE(chen::ip::address_v6("2001:db8:122:344::/96").isIPv4EmbeddedWellKnown());

    // operator
    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::200e"));
    EXPECT_EQ(chen::ip::address_v6("2404:6800:4004:817::200e/64"), chen::ip::address_v6("2404:6800:4004:817::200e/64"));

    EXPECT_NE(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::"));
    EXPECT_NE(chen::ip::address_v6("2404:6800:4004:817::200e/64"), chen::ip::address_v6("2404:6800:4004:817::200e"));

    EXPECT_LT(chen::ip::address_v6("2404:6800:4004:817::"), chen::ip::address_v6("2404:6800:4004:817::200e"));
    EXPECT_LT(chen::ip::address_v6("2404:6800:4004:817::200e/64"), chen::ip::address_v6("2404:6800:4004:817::200e"));

    EXPECT_GT(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::"));
    EXPECT_GT(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::200e/64"));

    EXPECT_LE(chen::ip::address_v6("2404:6800:4004:817::"), chen::ip::address_v6("2404:6800:4004:817::200e"));
    EXPECT_LE(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::200e"));
    EXPECT_LE(chen::ip::address_v6("2404:6800:4004:817::200e/64"), chen::ip::address_v6("2404:6800:4004:817::200e"));

    EXPECT_GE(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::"));
    EXPECT_GE(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::200e"));
    EXPECT_GE(chen::ip::address_v6("2404:6800:4004:817::200e"), chen::ip::address_v6("2404:6800:4004:817::200e/64"));

    // common
    EXPECT_EQ("::", chen::ip::address_v6::any().str());
    EXPECT_EQ("::1", chen::ip::address_v6::loopback().str());

    // invalid test
    EXPECT_THROW(chen::ip::address_v6("2404:6800:4004:817::200e", 200), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v6(bytes, 200), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v6::toBytes("::1::1"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v6::toBytes("::192.fe:1:1"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v6::toBytes("::1^$"), chen::ip::address::error);
    EXPECT_THROW(chen::ip::address_v6::toBytes("::1/200"), chen::ip::address::error);
}