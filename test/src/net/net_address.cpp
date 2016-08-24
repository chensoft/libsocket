/**
 * Created by Jian Chen
 * @since  2016.05.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_interface.hpp>
#include <socket/net/net_error.hpp>
#include <gtest/gtest.h>

TEST(NetAddressTest, Base)
{
    using chen::net::address;

    EXPECT_TRUE(address(nullptr).empty());
    EXPECT_FALSE(address(nullptr));

    EXPECT_EQ(address::Type::IPv4, address::detect("127.0.0.1"));
    EXPECT_EQ(address::Type::IPv6, address::detect("2404:6800:4004:817::200e"));
}

TEST(NetAddressTest, IPv4)
{
    using chen::net::address;
    using chen::net::version4;
    using chen::net::error_address;

    // assign
    EXPECT_EQ(address(address::Type::IPv4), address("0.0.0.0"));

    EXPECT_EQ(address("127.0.0.1/8"), address("127.0.0.1/8"));
    EXPECT_EQ(address("127.0.0.1/8"), address("127.0.0.1", 8));
    EXPECT_EQ(address("127.0.0.1/8"), address("127.0.0.1", "255.0.0.0"));

    EXPECT_EQ(address("127"), address("127.0.0.0"));
    EXPECT_EQ(address("127.1"), address("127.0.0.1"));
    EXPECT_EQ(address("192.168.1"), address("192.168.0.1"));

    EXPECT_EQ(address(0x7F000001), address("127.0.0.1"));
    EXPECT_EQ(address(version4(0x7F000001, 8)), address("127.0.0.1/8"));
    EXPECT_EQ(address(version4(0x7F000001, "255.0.0.0")), address("127.0.0.1/8"));

    EXPECT_THROW(address("127.0.0.1", 33), error_address);
    EXPECT_THROW(address(version4(0x7F000001, 33)), error_address);

    address v4("127");
    EXPECT_EQ("127.0.0.0", v4.str());

    v4 = "192.168.1.1/24";
    EXPECT_EQ("192.168.1.1/24", v4.str(true));
    EXPECT_EQ(24, v4.cidr());

    v4 = 0x7F000001;
    EXPECT_EQ("127.0.0.1", v4.str());
    EXPECT_EQ(32, v4.cidr());

    // representation
    EXPECT_EQ("127.0.0.1", address("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1", address("127.0.1").str());
    EXPECT_EQ("127.0.0.1", address("127.1").str());
    EXPECT_EQ("127.0.0.1", address(0x7F000001).str());

    EXPECT_EQ("127.0.0.1", address("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1/8", address("127.0.0.1/8").str(true));
    EXPECT_EQ("127.0.0.1/8", address(version4(0x7F000001, 8)).str(true));
    EXPECT_EQ("192.168.1.1/30", address("192.168.1.1", 30).str(true));

    // network
    EXPECT_EQ(8, address("127.0.0.1/8").cidr());
    EXPECT_EQ(16, address("180.166.92.36/16").cidr());
    EXPECT_EQ(24, address("192.168.0.1/24").cidr());
    EXPECT_EQ(26, address("192.168.0.1/26").cidr());

    EXPECT_EQ(address("255.0.0.0").v4().addr(), address("127.0.0.1/8").v4().netmask());
    EXPECT_EQ(address("0.255.255.255").v4().addr(), address("127.0.0.1/8").v4().wildcard());

    EXPECT_EQ("127.0.0.0/8", address("127.0.0.1/8").network().str(true));
    EXPECT_EQ("127.0.0.1/8", address("127.0.0.1/8").minhost().str(true));
    EXPECT_EQ("127.255.255.254/8", address("127.0.0.1/8").maxhost().str(true));
    EXPECT_EQ("127.255.255.255/8", address("127.0.0.1/8").v4().broadcast().str(true));
    EXPECT_EQ(16777214u, address("127.0.0.1/8").v4().hosts());

    // special
    EXPECT_TRUE(address("0.0.0.0").v4().isReserved());
    EXPECT_TRUE(address("127.0.0.1").v4().isReserved());
    EXPECT_TRUE(address("169.254.0.0").v4().isReserved());
    EXPECT_TRUE(address("192.0.0.0").v4().isReserved());
    EXPECT_TRUE(address("192.0.2.0").v4().isReserved());
    EXPECT_TRUE(address("192.88.99.0").v4().isReserved());
    EXPECT_TRUE(address("198.51.100.0").v4().isReserved());
    EXPECT_TRUE(address("203.0.113.0").v4().isReserved());
    EXPECT_TRUE(address("224.0.0.0").v4().isReserved());
    EXPECT_TRUE(address("240.0.0.0").v4().isReserved());
    EXPECT_TRUE(address("10.0.0.0").v4().isReserved());
    EXPECT_TRUE(address("100.64.0.0").v4().isReserved());
    EXPECT_TRUE(address("172.16.0.0").v4().isReserved());
    EXPECT_TRUE(address("192.168.0.0").v4().isReserved());
    EXPECT_TRUE(address("198.18.0.0").v4().isReserved());

    EXPECT_TRUE(address("10.0.0.0").v4().isPrivate());
    EXPECT_TRUE(address("100.64.0.0").v4().isPrivate());
    EXPECT_TRUE(address("172.16.0.0").v4().isPrivate());
    EXPECT_TRUE(address("192.168.0.0").v4().isPrivate());
    EXPECT_TRUE(address("198.18.0.0").v4().isPrivate());

    EXPECT_TRUE(address("127.0.0.1").isLoopback());
    EXPECT_FALSE(address("192.168.0.1").isLoopback());

    EXPECT_TRUE(address("169.254.0.0").v4().isLinkLocal());

    EXPECT_TRUE(address("224.0.0.115").isMulticast());
    EXPECT_FALSE(address("10.0.0.1").isMulticast());

    EXPECT_TRUE(address("192.168.0.255/24").v4().isBroadcast());
    EXPECT_FALSE(address("192.168.0.1/24").v4().isBroadcast());

    // class A
    EXPECT_TRUE(address("0.0.0.0").v4().isClassA());
    EXPECT_FALSE(address("0.0.0.0").v4().isClassB());
    EXPECT_FALSE(address("0.0.0.0").v4().isClassC());

    EXPECT_TRUE(address("10.0.0.0").v4().isClassA());
    EXPECT_FALSE(address("10.0.0.0").v4().isClassB());
    EXPECT_FALSE(address("10.0.0.0").v4().isClassC());

    EXPECT_TRUE(address("127.255.255.255").v4().isClassA());
    EXPECT_FALSE(address("127.255.255.255").v4().isClassB());
    EXPECT_FALSE(address("127.255.255.255").v4().isClassC());

    // class B
    EXPECT_FALSE(address("128.0.0.0").v4().isClassA());
    EXPECT_TRUE(address("128.0.0.0").v4().isClassB());
    EXPECT_FALSE(address("128.0.0.0").v4().isClassC());

    EXPECT_FALSE(address("190.0.0.0").v4().isClassA());
    EXPECT_TRUE(address("190.0.0.0").v4().isClassB());
    EXPECT_FALSE(address("190.0.0.0").v4().isClassC());

    EXPECT_FALSE(address("191.255.255.255").v4().isClassA());
    EXPECT_TRUE(address("191.255.255.255").v4().isClassB());
    EXPECT_FALSE(address("191.255.255.255").v4().isClassC());

    // class C
    EXPECT_FALSE(address("192.0.0.0").v4().isClassA());
    EXPECT_FALSE(address("192.0.0.0").v4().isClassB());
    EXPECT_TRUE(address("192.0.0.0").v4().isClassC());

    EXPECT_FALSE(address("192.168.1.1").v4().isClassA());
    EXPECT_FALSE(address("192.168.1.1").v4().isClassB());
    EXPECT_TRUE(address("192.168.1.1").v4().isClassC());

    EXPECT_FALSE(address("223.255.255.255").v4().isClassA());
    EXPECT_FALSE(address("223.255.255.255").v4().isClassB());
    EXPECT_TRUE(address("223.255.255.255").v4().isClassC());

    // class D & E
    EXPECT_TRUE(address("224.0.0.0").v4().isClassD());
    EXPECT_TRUE(address("239.255.255.255").v4().isClassD());

    EXPECT_TRUE(address("240.0.0.0").v4().isClassE());
    EXPECT_TRUE(address("255.255.255.255").v4().isClassE());

    // operator
    EXPECT_EQ(address("127.0.0.1"), address("127.0.0.1"));
    EXPECT_EQ(address("127.0.0.1/8"), address("127.0.0.1/8"));

    EXPECT_NE(address("127.0.0.0"), address("127.0.0.1"));
    EXPECT_NE(address("127.0.0.1/8"), address("127.0.0.1"));

    EXPECT_LT(address("127.0.0.0"), address("127.0.0.1"));
    EXPECT_LT(address("127.0.0.1/8"), address("127.0.0.1"));

    EXPECT_GT(address("127.0.0.1"), address("127.0.0.0"));
    EXPECT_GT(address("127.0.0.1"), address("127.0.0.1/8"));

    EXPECT_LE(address("127.0.0.0"), address("127.0.0.1"));
    EXPECT_LE(address("127.0.0.1"), address("127.0.0.1"));
    EXPECT_LE(address("127.0.0.1/8"), address("127.0.0.1"));

    EXPECT_GE(address("127.0.0.1"), address("127.0.0.0"));
    EXPECT_GE(address("127.0.0.1"), address("127.0.0.1"));
    EXPECT_GE(address("127.0.0.1"), address("127.0.0.1/8"));

    // common
    EXPECT_EQ("0.0.0.0", address::any(address::Type::IPv4).str());
    EXPECT_EQ("127.0.0.1", address::loopback(address::Type::IPv4).str());

    // invalid test
    std::uint8_t cidr = 0;

    EXPECT_THROW(version4::toInteger("127..1"), error_address);
    EXPECT_THROW(version4::toInteger("999.0.0.0"), error_address);
    EXPECT_THROW(version4::toInteger("127.0.0.1/99", &cidr), error_address);
}

TEST(NetAddressTest, IPv6)
{
    using chen::net::address;
    using chen::net::version6;
    using chen::net::interface;
    using chen::net::error_address;

    // assign
    EXPECT_EQ(address(address::Type::IPv6), address("::"));
    EXPECT_EQ(address("::1/64"), address("::1/64"));
    EXPECT_EQ(address("::1/64"), address("::1", 64));
    EXPECT_EQ(address("::1/64"), address("::1", "ffff:ffff:ffff:ffff::"));

    EXPECT_EQ(address("::"), address("0000:0000:0000:0000:0000:0000:0000:0000"));
    EXPECT_EQ(address("::"), address("0:0:0:0:0:0:0:0"));

    EXPECT_EQ(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817:0:0:0:200e"));
    EXPECT_EQ(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817:0000:0000:0000:200e"));
    EXPECT_EQ(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::200e/128"));

    EXPECT_EQ(address("::c0a8:1"), address("::192.168.0.1"));

    EXPECT_EQ(address("fe80::1%1"), address("fe80::1%1"));
    EXPECT_EQ(address("fe80::1%1"), address("fe80::1%1", 128));
    EXPECT_EQ(address("fe80::1%1"), address("fe80::1", 128, 1));

    EXPECT_EQ(address("fe80::1%1/64"), address("fe80::1%1", 64));
    EXPECT_EQ(address("fe80::1%1/64"), address("fe80::1", 64, 1));
    EXPECT_EQ(address("fe80::1%1/64"), address("fe80::1%1", "ffff:ffff:ffff:ffff::"));
    EXPECT_EQ(address("fe80::1%1/64"), address("fe80::1", "ffff:ffff:ffff:ffff::", 1));

    std::uint8_t bytes[16] = {0x24, 0x04, 0x68, 0, 0x40, 0x04, 0x08, 0x17, 0, 0, 0, 0, 0, 0, 0x20, 0x0e};

    EXPECT_EQ(address("2404:6800:4004:817::200e"), address(bytes));
    EXPECT_EQ(address("2404:6800:4004:817::200e/64"), address(version6(bytes, 64)));
    EXPECT_EQ(address("2404:6800:4004:817::200e/64"), address(version6(bytes, "ffff:ffff:ffff:ffff::")));

    EXPECT_THROW(address("2404:6800:4004:817::200e/129"), error_address);
    EXPECT_THROW(address(version6(bytes, 129)), error_address);

    address v6("::1");
    EXPECT_EQ("::1", v6.str());

    v6 = "2404:6800:4004:817::200e";
    EXPECT_EQ("2404:6800:4004:817::200e", v6.str());
    EXPECT_EQ(128, v6.cidr());

    v6 = bytes;
    EXPECT_EQ("2404:6800:4004:817::200e", v6.str());
    EXPECT_EQ(128, v6.cidr());

    // uppercase
    EXPECT_EQ(address("fe80::1%1"), address("FE80::1%1"));

    // representation
    EXPECT_EQ("::", address(address::Type::IPv6).str());
    EXPECT_EQ("2404:6800:4004:817::", address("2404:6800:4004:817:0000:0000:0000:0000").str());
    EXPECT_EQ("2404::817:0:0:0:200e", address("2404:0000:0000:817:0000:0000:0000:200e").str());
    EXPECT_EQ("2404:6800:4004:817::200e", address("2404:6800:4004:817:0000:0000:0000:200e").str());
    EXPECT_EQ("2404:6800:4004:0817:0000:0000:0000:200e", address("2404:6800:4004:817:0000:0000:0000:200e").v6().expanded());
    EXPECT_EQ("2404:6800:4004:817:0:0:0:200e", address("2404:6800:4004:817:0000:0000:0000:200e").v6().suppressed());
    EXPECT_EQ("2404:6800:4004:817::200e", address("2404:6800:4004:817:0000:0000:0000:200e").v6().compressed());
    EXPECT_EQ("::ffff:192.0.2.128", address("::ffff:c000:280").v6().mixed());

    auto scope = interface::scope(1);

    EXPECT_EQ("fe80::1%" + scope, address("fe80::1%1").v6().str(false, true));
    EXPECT_EQ("fe80::1%" + scope + "/128", address("fe80::1%1").v6().str(true, true));

    EXPECT_EQ(address("192.0.2.128").v4(), address("::ffff:c000:280").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("2001:db8:c000:221::/32").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("2001:db8:1c0:2:21::/40").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("2001:db8:122:c000:2:2100::/48").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("2001:db8:122:3c0:0:221::/56").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("2001:db8:122:344:c0:2:2100::/64").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("2001:db8:122:344::192.0.2.33/96").v6().embedded());
    EXPECT_EQ(address("192.0.2.33").v4(), address("64:ff9b::192.0.2.33/96").v6().embedded());

    EXPECT_EQ("::", address(address::Type::IPv6).str());
    EXPECT_EQ("2404:6800:4004:817::200e", address("2404:6800:4004:817:0000:0000:0000:200e").str());

    std::array<std::uint8_t, 16> tmp;
    ::memcpy(tmp.data(), bytes, 16);

    EXPECT_EQ(tmp, address("2404:6800:4004:817::200e").v6().addr());
    EXPECT_EQ(128, address("2404:6800:4004:817::200e").cidr());
    EXPECT_EQ(128, address("2404:6800:4004:817::200e/128").cidr());
    EXPECT_EQ(64, address("2404:6800:4004:817::200e/64").cidr());

    // network
    std::array<std::uint8_t, 16> netmask = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    std::array<std::uint8_t, 16> wildcard = {{0, 0, 0, 0, 0, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, address("2404:6800:4004:817::200e/48").v6().netmask());
    EXPECT_EQ(wildcard, address("2404:6800:4004:817::200e/48").v6().wildcard());

    netmask  = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    wildcard = {{0, 0, 0, 0, 0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

    EXPECT_EQ(netmask, address("2404:6800:4004:817::200e/46").v6().netmask());
    EXPECT_EQ(wildcard, address("2404:6800:4004:817::200e/46").v6().wildcard());

    EXPECT_EQ("2404:6800:4004::/46", address("2404:6800:4004:817::200e/46").network().str(true));
    EXPECT_EQ("2404:6800:4004:817::200e", address("2404:6800:4004:817::200e/128").network().str());

    EXPECT_EQ("2404:6800:4004::/46", address("2404:6800:4004:817::200e/46").minhost().str(true));
    EXPECT_EQ("2404:6800:4007:ffff:ffff:ffff:ffff:ffff/46", address("2404:6800:4004:817::200e/46").maxhost().str(true));

    EXPECT_EQ("2404:6800:4004:817::200e", address("2404:6800:4004:817::200e/128").minhost().str());
    EXPECT_EQ("2404:6800:4004:817::200e", address("2404:6800:4004:817::200e/128").maxhost().str());

    // special
    EXPECT_TRUE(address("::").isUnspecified());
    EXPECT_FALSE(address("::1").isUnspecified());

    EXPECT_TRUE(address("::1").isLoopback());
    EXPECT_FALSE(address("::").isLoopback());

    EXPECT_TRUE(address("2404:6800:4004:817::200e").v6().isGlobalUnicast());
    EXPECT_FALSE(address("fe80::7a31:c1ff:fec2:b5aa").v6().isGlobalUnicast());

    EXPECT_TRUE(address("fe80::7a31:c1ff:fec2:b5aa").v6().isLinkLocalUnicast());
    EXPECT_FALSE(address("2404:6800:4004:817::200e").v6().isLinkLocalUnicast());

    EXPECT_TRUE(address("FEC0::1234:5678:9ABC").v6().isSiteLocalUnicast());
    EXPECT_FALSE(address("2404:6800:4004:817::200e").v6().isSiteLocalUnicast());

    EXPECT_TRUE(address("::192.168.1.1").v6().isIPv4Compatible());
    EXPECT_FALSE(address("2404::192.168.1.1").v6().isIPv4Compatible());

    EXPECT_TRUE(address("::ffff:192.168.1.1").v6().isIPv4Mapped());
    EXPECT_FALSE(address("::192.168.1.1").v6().isIPv4Mapped());

    EXPECT_TRUE(address("::ffff:192.168.1.1").v6().isIPv4Mapped());
    EXPECT_FALSE(address("::192.168.1.1").v6().isIPv4Mapped());

    EXPECT_TRUE(address("ff02::1").isMulticast());
    EXPECT_FALSE(address("2404::192.168.1.1").isMulticast());

    EXPECT_TRUE(address("64:ff9b::73b6:405e").v6().isIPv4EmbeddedWellKnown());
    EXPECT_FALSE(address("2001:db8:122:344::").v6().isIPv4EmbeddedWellKnown());

    // operator
    EXPECT_EQ(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::200e"));
    EXPECT_EQ(address("2404:6800:4004:817::200e/64"), address("2404:6800:4004:817::200e/64"));

    EXPECT_NE(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::"));
    EXPECT_NE(address("2404:6800:4004:817::200e/64"), address("2404:6800:4004:817::200e"));

    EXPECT_LT(address("2404:6800:4004:817::"), address("2404:6800:4004:817::200e"));
    EXPECT_LT(address("2404:6800:4004:817::200e/64"), address("2404:6800:4004:817::200e"));

    EXPECT_GT(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::"));
    EXPECT_GT(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::200e/64"));

    EXPECT_LE(address("2404:6800:4004:817::"), address("2404:6800:4004:817::200e"));
    EXPECT_LE(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::200e"));
    EXPECT_LE(address("2404:6800:4004:817::200e/64"), address("2404:6800:4004:817::200e"));

    EXPECT_GE(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::"));
    EXPECT_GE(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::200e"));
    EXPECT_GE(address("2404:6800:4004:817::200e"), address("2404:6800:4004:817::200e/64"));

    // common
    EXPECT_EQ("::", address::any(address::Type::IPv6).str());
    EXPECT_EQ("::1", address::loopback(address::Type::IPv6).str());

    // invalid test
    std::uint8_t cidr = 0;

    EXPECT_THROW(address("2404:6800:4004:817::200e", 200), error_address);
    EXPECT_THROW(address(version6(bytes, 200)), error_address);
    EXPECT_THROW(version6::toBytes("::1::1"), error_address);
    EXPECT_THROW(version6::toBytes("::192.fe:1:1"), error_address);
    EXPECT_THROW(version6::toBytes("::1^$"), error_address);
    EXPECT_THROW(version6::toBytes("::1/200", &cidr), error_address);
}