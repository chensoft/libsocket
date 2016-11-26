/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_adapter.hpp>
#include <gtest/gtest.h>

using chen::ip_address;
using chen::ip_version4;
using chen::ip_version6;
using chen::inet_adapter;

TEST(IPAddressTest, Base)
{
    EXPECT_TRUE(ip_address(nullptr).empty());
    EXPECT_FALSE(ip_address(nullptr));

    EXPECT_EQ(ip_address::Type::IPv4, ip_address::detect("127.0.0.1"));
    EXPECT_EQ(ip_address::Type::IPv6, ip_address::detect("2404:6800:4004:817::200e"));
}

TEST(IPAddressTest, IPv4)
{
    // assign
    EXPECT_EQ(ip_address(ip_address::Type::IPv4), ip_address("0.0.0.0"));

    EXPECT_EQ(ip_address("127.0.0.1/8"), ip_address("127.0.0.1/8"));
    EXPECT_EQ(ip_address("127.0.0.1/8"), ip_address("127.0.0.1", 8));
    EXPECT_EQ(ip_address("127.0.0.1/8"), ip_address("127.0.0.1", "255.0.0.0"));

    EXPECT_EQ(ip_address("127"), ip_address("127.0.0.0"));
    EXPECT_EQ(ip_address("127.1"), ip_address("127.0.0.1"));
    EXPECT_EQ(ip_address("192.168.1"), ip_address("192.168.0.1"));

    EXPECT_EQ(ip_address(ip_version4(0x7F000001)), ip_address("127.0.0.1"));
    EXPECT_EQ(ip_address(ip_version4(0x7F000001, 8)), ip_address("127.0.0.1/8"));
    EXPECT_EQ(ip_address(ip_version4(0x7F000001, "255.0.0.0")), ip_address("127.0.0.1/8"));

    EXPECT_THROW(ip_address("127.0.0.1", 33), std::runtime_error);
    EXPECT_THROW(ip_address(ip_version4(0x7F000001, 33)), std::runtime_error);

    ip_address v4("127");
    EXPECT_EQ("127.0.0.0", v4.str());

    v4 = "192.168.1.1/24";
    EXPECT_EQ("192.168.1.1/24", v4.str(true));
    EXPECT_EQ(24, v4.cidr());

    v4 = ip_version4(0x7F000001);
    EXPECT_EQ("127.0.0.1", v4.str());
    EXPECT_EQ(32, v4.cidr());

    // representation
    EXPECT_EQ("127.0.0.1", ip_address("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1", ip_address("127.0.1").str());
    EXPECT_EQ("127.0.0.1", ip_address("127.1").str());
    EXPECT_EQ("127.0.0.1", ip_address(ip_version4(0x7F000001)).str());

    EXPECT_EQ("127.0.0.1", ip_address("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1/8", ip_address("127.0.0.1/8").str(true));
    EXPECT_EQ("127.0.0.1/8", ip_address(ip_version4(0x7F000001, 8)).str(true));
    EXPECT_EQ("192.168.1.1/30", ip_address("192.168.1.1", 30).str(true));

    // network
    EXPECT_EQ(8, ip_address("127.0.0.1/8").cidr());
    EXPECT_EQ(16, ip_address("180.166.92.36/16").cidr());
    EXPECT_EQ(24, ip_address("192.168.0.1/24").cidr());
    EXPECT_EQ(26, ip_address("192.168.0.1/26").cidr());

    EXPECT_EQ(ip_address("255.0.0.0").v4().addr(), ip_address("127.0.0.1/8").v4().netmask());
    EXPECT_EQ(ip_address("0.255.255.255").v4().addr(), ip_address("127.0.0.1/8").v4().wildcard());

    EXPECT_EQ("127.0.0.0/8", ip_address("127.0.0.1/8").network().str(true));
    EXPECT_EQ("127.0.0.1/8", ip_address("127.0.0.1/8").minhost().str(true));
    EXPECT_EQ("127.255.255.254/8", ip_address("127.0.0.1/8").maxhost().str(true));
    EXPECT_EQ("127.255.255.255/8", ip_address("127.0.0.1/8").v4().broadcast().str(true));
    EXPECT_EQ(16777214u, ip_address("127.0.0.1/8").v4().hosts());

    // special
    EXPECT_TRUE(ip_address("0.0.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("127.0.0.1").v4().isReserved());
    EXPECT_TRUE(ip_address("169.254.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("192.0.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("192.0.2.0").v4().isReserved());
    EXPECT_TRUE(ip_address("192.88.99.0").v4().isReserved());
    EXPECT_TRUE(ip_address("198.51.100.0").v4().isReserved());
    EXPECT_TRUE(ip_address("203.0.113.0").v4().isReserved());
    EXPECT_TRUE(ip_address("224.0.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("240.0.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("10.0.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("100.64.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("172.16.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("192.168.0.0").v4().isReserved());
    EXPECT_TRUE(ip_address("198.18.0.0").v4().isReserved());

    EXPECT_TRUE(ip_address("10.0.0.0").v4().isPrivate());
    EXPECT_TRUE(ip_address("100.64.0.0").v4().isPrivate());
    EXPECT_TRUE(ip_address("172.16.0.0").v4().isPrivate());
    EXPECT_TRUE(ip_address("192.168.0.0").v4().isPrivate());
    EXPECT_TRUE(ip_address("198.18.0.0").v4().isPrivate());

    EXPECT_TRUE(ip_address("127.0.0.1").isLoopback());
    EXPECT_FALSE(ip_address("192.168.0.1").isLoopback());

    EXPECT_TRUE(ip_address("169.254.0.0").v4().isLinkLocal());

    EXPECT_TRUE(ip_address("224.0.0.115").isMulticast());
    EXPECT_FALSE(ip_address("10.0.0.1").isMulticast());

    EXPECT_TRUE(ip_address("192.168.0.255/24").v4().isBroadcast());
    EXPECT_FALSE(ip_address("192.168.0.1/24").v4().isBroadcast());

    // class A
    EXPECT_TRUE(ip_address("0.0.0.0").v4().isClassA());
    EXPECT_FALSE(ip_address("0.0.0.0").v4().isClassB());
    EXPECT_FALSE(ip_address("0.0.0.0").v4().isClassC());

    EXPECT_TRUE(ip_address("10.0.0.0").v4().isClassA());
    EXPECT_FALSE(ip_address("10.0.0.0").v4().isClassB());
    EXPECT_FALSE(ip_address("10.0.0.0").v4().isClassC());

    EXPECT_TRUE(ip_address("127.255.255.255").v4().isClassA());
    EXPECT_FALSE(ip_address("127.255.255.255").v4().isClassB());
    EXPECT_FALSE(ip_address("127.255.255.255").v4().isClassC());

    // class B
    EXPECT_FALSE(ip_address("128.0.0.0").v4().isClassA());
    EXPECT_TRUE(ip_address("128.0.0.0").v4().isClassB());
    EXPECT_FALSE(ip_address("128.0.0.0").v4().isClassC());

    EXPECT_FALSE(ip_address("190.0.0.0").v4().isClassA());
    EXPECT_TRUE(ip_address("190.0.0.0").v4().isClassB());
    EXPECT_FALSE(ip_address("190.0.0.0").v4().isClassC());

    EXPECT_FALSE(ip_address("191.255.255.255").v4().isClassA());
    EXPECT_TRUE(ip_address("191.255.255.255").v4().isClassB());
    EXPECT_FALSE(ip_address("191.255.255.255").v4().isClassC());

    // class C
    EXPECT_FALSE(ip_address("192.0.0.0").v4().isClassA());
    EXPECT_FALSE(ip_address("192.0.0.0").v4().isClassB());
    EXPECT_TRUE(ip_address("192.0.0.0").v4().isClassC());

    EXPECT_FALSE(ip_address("192.168.1.1").v4().isClassA());
    EXPECT_FALSE(ip_address("192.168.1.1").v4().isClassB());
    EXPECT_TRUE(ip_address("192.168.1.1").v4().isClassC());

    EXPECT_FALSE(ip_address("223.255.255.255").v4().isClassA());
    EXPECT_FALSE(ip_address("223.255.255.255").v4().isClassB());
    EXPECT_TRUE(ip_address("223.255.255.255").v4().isClassC());

    // class D & E
    EXPECT_TRUE(ip_address("224.0.0.0").v4().isClassD());
    EXPECT_TRUE(ip_address("239.255.255.255").v4().isClassD());

    EXPECT_TRUE(ip_address("240.0.0.0").v4().isClassE());
    EXPECT_TRUE(ip_address("255.255.255.255").v4().isClassE());

    // operator
    EXPECT_EQ(ip_address("127.0.0.1"), ip_address("127.0.0.1"));
    EXPECT_EQ(ip_address("127.0.0.1/8"), ip_address("127.0.0.1/8"));

    EXPECT_NE(ip_address("127.0.0.0"), ip_address("127.0.0.1"));
    EXPECT_NE(ip_address("127.0.0.1/8"), ip_address("127.0.0.1"));

    EXPECT_LT(ip_address("127.0.0.0"), ip_address("127.0.0.1"));
    EXPECT_LT(ip_address("127.0.0.1/8"), ip_address("127.0.0.1"));

    EXPECT_GT(ip_address("127.0.0.1"), ip_address("127.0.0.0"));
    EXPECT_GT(ip_address("127.0.0.1"), ip_address("127.0.0.1/8"));

    EXPECT_LE(ip_address("127.0.0.0"), ip_address("127.0.0.1"));
    EXPECT_LE(ip_address("127.0.0.1"), ip_address("127.0.0.1"));
    EXPECT_LE(ip_address("127.0.0.1/8"), ip_address("127.0.0.1"));

    EXPECT_GE(ip_address("127.0.0.1"), ip_address("127.0.0.0"));
    EXPECT_GE(ip_address("127.0.0.1"), ip_address("127.0.0.1"));
    EXPECT_GE(ip_address("127.0.0.1"), ip_address("127.0.0.1/8"));

    // common
    EXPECT_EQ("0.0.0.0", ip_address::any(ip_address::Type::IPv4).str());
    EXPECT_EQ("127.0.0.1", ip_address::loopback(ip_address::Type::IPv4).str());

    // invalid test
    std::uint8_t cidr = 0;

    EXPECT_THROW(ip_version4::toInteger("127..1"), std::runtime_error);
    EXPECT_THROW(ip_version4::toInteger("999.0.0.0"), std::runtime_error);
    EXPECT_THROW(ip_version4::toInteger("127.0.0.1/99", &cidr), std::runtime_error);
}

TEST(IPAddressTest, IPv6)
{
    // assign
    EXPECT_EQ(ip_address(ip_address::Type::IPv6), ip_address("::"));
    EXPECT_EQ(ip_address("::1/64"), ip_address("::1/64"));
    EXPECT_EQ(ip_address("::1/64"), ip_address("::1", 64));
    EXPECT_EQ(ip_address("::1/64"), ip_address("::1", "ffff:ffff:ffff:ffff::"));

    EXPECT_EQ(ip_address("::"), ip_address("0000:0000:0000:0000:0000:0000:0000:0000"));
    EXPECT_EQ(ip_address("::"), ip_address("0:0:0:0:0:0:0:0"));

    EXPECT_EQ(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817:0:0:0:200e"));
    EXPECT_EQ(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817:0000:0000:0000:200e"));
    EXPECT_EQ(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::200e/128"));

    EXPECT_EQ(ip_address("::c0a8:1"), ip_address("::192.168.0.1"));

    EXPECT_EQ(ip_address("fe80::1%1"), ip_address("fe80::1%1"));
    EXPECT_EQ(ip_address("fe80::1%1"), ip_address("fe80::1%1", 128));
    EXPECT_EQ(ip_address("fe80::1%1"), ip_address("fe80::1", 128, 1));

    EXPECT_EQ(ip_address("fe80::1%1/64"), ip_address("fe80::1%1", 64));
    EXPECT_EQ(ip_address("fe80::1%1/64"), ip_address("fe80::1", 64, 1));
    EXPECT_EQ(ip_address("fe80::1%1/64"), ip_address("fe80::1%1", "ffff:ffff:ffff:ffff::"));
    EXPECT_EQ(ip_address("fe80::1%1/64"), ip_address("fe80::1", "ffff:ffff:ffff:ffff::", 1));

    std::uint8_t bytes[16] = {0x24, 0x04, 0x68, 0, 0x40, 0x04, 0x08, 0x17, 0, 0, 0, 0, 0, 0, 0x20, 0x0e};

    EXPECT_EQ(ip_address("2404:6800:4004:817::200e"), ip_address(ip_version6(bytes)));
    EXPECT_EQ(ip_address("2404:6800:4004:817::200e/64"), ip_address(ip_version6(bytes, 64)));
    EXPECT_EQ(ip_address("2404:6800:4004:817::200e/64"), ip_address(ip_version6(bytes, "ffff:ffff:ffff:ffff::")));

    EXPECT_THROW(ip_address("2404:6800:4004:817::200e/129"), std::runtime_error);
    EXPECT_THROW(ip_address(ip_version6(bytes, 129)), std::runtime_error);

    ip_address v6("::1");
    EXPECT_EQ("::1", v6.str());

    v6 = "2404:6800:4004:817::200e";
    EXPECT_EQ("2404:6800:4004:817::200e", v6.str());
    EXPECT_EQ(128, v6.cidr());

    v6 = ip_version6(bytes);
    EXPECT_EQ("2404:6800:4004:817::200e", v6.str());
    EXPECT_EQ(128, v6.cidr());

    // uppercase
    EXPECT_EQ(ip_address("fe80::1%1"), ip_address("FE80::1%1"));

    // representation
    EXPECT_EQ("::", ip_address(ip_address::Type::IPv6).str());
    EXPECT_EQ("2404:6800:4004:817::", ip_address("2404:6800:4004:817:0000:0000:0000:0000").str());
    EXPECT_EQ("2404::817:0:0:0:200e", ip_address("2404:0000:0000:817:0000:0000:0000:200e").str());
    EXPECT_EQ("2404:6800:4004:817::200e", ip_address("2404:6800:4004:817:0000:0000:0000:200e").str());
    EXPECT_EQ("2404:6800:4004:0817:0000:0000:0000:200e", ip_address("2404:6800:4004:817:0000:0000:0000:200e").v6().expanded());
    EXPECT_EQ("2404:6800:4004:817:0:0:0:200e", ip_address("2404:6800:4004:817:0000:0000:0000:200e").v6().suppressed());
    EXPECT_EQ("2404:6800:4004:817::200e", ip_address("2404:6800:4004:817:0000:0000:0000:200e").v6().compressed());
    EXPECT_EQ("::ffff:192.0.2.128", ip_address("::ffff:c000:280").v6().mixed());

    auto scope = inet_adapter::scope(1);

    EXPECT_EQ("fe80::1%" + scope, ip_address("fe80::1%1").v6().str(false, true));
    EXPECT_EQ("fe80::1%" + scope + "/128", ip_address("fe80::1%1").v6().str(true, true));

    EXPECT_EQ(ip_address("192.0.2.128").v4(), ip_address("::ffff:c000:280").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("2001:db8:c000:221::/32").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("2001:db8:1c0:2:21::/40").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("2001:db8:122:c000:2:2100::/48").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("2001:db8:122:3c0:0:221::/56").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("2001:db8:122:344:c0:2:2100::/64").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("2001:db8:122:344::192.0.2.33/96").v6().embedded());
    EXPECT_EQ(ip_address("192.0.2.33").v4(), ip_address("64:ff9b::192.0.2.33/96").v6().embedded());

    EXPECT_EQ("::", ip_address(ip_address::Type::IPv6).str());
    EXPECT_EQ("2404:6800:4004:817::200e", ip_address("2404:6800:4004:817:0000:0000:0000:200e").str());

    std::array<std::uint8_t, 16> tmp;
    ::memcpy(tmp.data(), bytes, 16);

    EXPECT_EQ(tmp, ip_address("2404:6800:4004:817::200e").v6().addr());
    EXPECT_EQ(128, ip_address("2404:6800:4004:817::200e").cidr());
    EXPECT_EQ(128, ip_address("2404:6800:4004:817::200e/128").cidr());
    EXPECT_EQ(64, ip_address("2404:6800:4004:817::200e/64").cidr());

    // network
    std::array<std::uint8_t, 16> netmask = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    std::array<std::uint8_t, 16> wildcard = {{0, 0, 0, 0, 0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, ip_address("2404:6800:4004:817::200e/48").v6().netmask());
    EXPECT_EQ(wildcard, ip_address("2404:6800:4004:817::200e/48").v6().wildcard());

    netmask  = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    wildcard = {{0, 0, 0, 0, 0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, ip_address("2404:6800:4004:817::200e/46").v6().netmask());
    EXPECT_EQ(wildcard, ip_address("2404:6800:4004:817::200e/46").v6().wildcard());

    EXPECT_EQ("2404:6800:4004::/46", ip_address("2404:6800:4004:817::200e/46").network().str(true));
    EXPECT_EQ("2404:6800:4004:817::200e", ip_address("2404:6800:4004:817::200e/128").network().str());

    EXPECT_EQ("2404:6800:4004::/46", ip_address("2404:6800:4004:817::200e/46").minhost().str(true));
    EXPECT_EQ("2404:6800:4007:ffff:ffff:ffff:ffff:ffff/46", ip_address("2404:6800:4004:817::200e/46").maxhost().str(true));

    EXPECT_EQ("2404:6800:4004:817::200e", ip_address("2404:6800:4004:817::200e/128").minhost().str());
    EXPECT_EQ("2404:6800:4004:817::200e", ip_address("2404:6800:4004:817::200e/128").maxhost().str());

    // special
    EXPECT_TRUE(ip_address("::").isUnspecified());
    EXPECT_FALSE(ip_address("::1").isUnspecified());

    EXPECT_TRUE(ip_address("::1").isLoopback());
    EXPECT_FALSE(ip_address("::").isLoopback());

    EXPECT_TRUE(ip_address("2404:6800:4004:817::200e").v6().isGlobalUnicast());
    EXPECT_FALSE(ip_address("fe80::7a31:c1ff:fec2:b5aa").v6().isGlobalUnicast());

    EXPECT_TRUE(ip_address("fe80::7a31:c1ff:fec2:b5aa").v6().isLinkLocalUnicast());
    EXPECT_FALSE(ip_address("2404:6800:4004:817::200e").v6().isLinkLocalUnicast());

    EXPECT_TRUE(ip_address("FEC0::1234:5678:9ABC").v6().isSiteLocalUnicast());
    EXPECT_FALSE(ip_address("2404:6800:4004:817::200e").v6().isSiteLocalUnicast());

    EXPECT_TRUE(ip_address("::192.168.1.1").v6().isIPv4Compatible());
    EXPECT_FALSE(ip_address("2404::192.168.1.1").v6().isIPv4Compatible());

    EXPECT_TRUE(ip_address("::ffff:192.168.1.1").v6().isIPv4Mapped());
    EXPECT_FALSE(ip_address("::192.168.1.1").v6().isIPv4Mapped());

    EXPECT_TRUE(ip_address("::ffff:192.168.1.1").v6().isIPv4Mapped());
    EXPECT_FALSE(ip_address("::192.168.1.1").v6().isIPv4Mapped());

    EXPECT_TRUE(ip_address("ff02::1").isMulticast());
    EXPECT_FALSE(ip_address("2404::192.168.1.1").isMulticast());

    EXPECT_TRUE(ip_address("64:ff9b::73b6:405e").v6().isIPv4EmbeddedWellKnown());
    EXPECT_FALSE(ip_address("2001:db8:122:344::").v6().isIPv4EmbeddedWellKnown());

    // operator
    EXPECT_EQ(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::200e"));
    EXPECT_EQ(ip_address("2404:6800:4004:817::200e/64"), ip_address("2404:6800:4004:817::200e/64"));

    EXPECT_NE(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::"));
    EXPECT_NE(ip_address("2404:6800:4004:817::200e/64"), ip_address("2404:6800:4004:817::200e"));

    EXPECT_LT(ip_address("2404:6800:4004:817::"), ip_address("2404:6800:4004:817::200e"));
    EXPECT_LT(ip_address("2404:6800:4004:817::200e/64"), ip_address("2404:6800:4004:817::200e"));

    EXPECT_GT(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::"));
    EXPECT_GT(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::200e/64"));

    EXPECT_LE(ip_address("2404:6800:4004:817::"), ip_address("2404:6800:4004:817::200e"));
    EXPECT_LE(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::200e"));
    EXPECT_LE(ip_address("2404:6800:4004:817::200e/64"), ip_address("2404:6800:4004:817::200e"));

    EXPECT_GE(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::"));
    EXPECT_GE(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::200e"));
    EXPECT_GE(ip_address("2404:6800:4004:817::200e"), ip_address("2404:6800:4004:817::200e/64"));

    // common
    EXPECT_EQ("::", ip_address::any(ip_address::Type::IPv6).str());
    EXPECT_EQ("::1", ip_address::loopback(ip_address::Type::IPv6).str());

    // invalid test
    std::uint8_t cidr = 0;

    EXPECT_THROW(ip_address("2404:6800:4004:817::200e", 200), std::runtime_error);
    EXPECT_THROW(ip_address(ip_version6(bytes, 200)), std::runtime_error);
    EXPECT_THROW(ip_version6::toBytes("::1::1"), std::runtime_error);
    EXPECT_THROW(ip_version6::toBytes("::192.fe:1:1"), std::runtime_error);
    EXPECT_THROW(ip_version6::toBytes("::1^$"), std::runtime_error);
    EXPECT_THROW(ip_version6::toBytes("::1/200", &cidr), std::runtime_error);
}