/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/inet/inet_address.hpp"
#include "chen/base/num.hpp"
#include "gtest/gtest.h"

using chen::ip_address;
using chen::ip_version6;
using chen::inet_address;
using chen::basic_address;

TEST(InetAddressTest, General)
{
    // nullptr
    EXPECT_TRUE(inet_address(nullptr).empty());
    EXPECT_FALSE(inet_address(nullptr));
    EXPECT_EQ("", inet_address().str());

    // one param: string
    EXPECT_EQ("0.0.0.0:80", inet_address(":80").str());
    EXPECT_EQ("127.0.0.1:0", inet_address("127.0.0.1").str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1:80").str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1:http").str());

    EXPECT_EQ("[::]:80", inet_address("[::]:80").str());
    EXPECT_EQ("[fe80::1]:0", inet_address("[fe80::1]").str());
    EXPECT_EQ("[fe80::1]:80", inet_address("[fe80::1]:80").str());
    EXPECT_EQ("[fe80::1]:80", inet_address("[fe80::1]:http").str());

    // addr + port|service
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1", 80).str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1", "80").str());
    EXPECT_EQ("127.0.0.1:80", inet_address("127.0.0.1", "http").str());

    EXPECT_EQ("[::]:80", inet_address("::", 80).str());
    EXPECT_EQ("[fe80::1]:80", inet_address("fe80::1", "80").str());
    EXPECT_EQ("[fe80::1]:80", inet_address("fe80::1", "http").str());

    // raw socket address
    ::sockaddr_storage v4{};
    ::sockaddr_storage v6{};

    ::sockaddr_in  *p4 = (::sockaddr_in*)&v4;
    ::sockaddr_in6 *p6 = (::sockaddr_in6*)&v6;

    p4->sin_family = AF_INET;
    p4->sin_port   = chen::num::swap(static_cast<std::uint16_t>(80));
    p4->sin_addr.s_addr = chen::num::swap(static_cast<std::uint32_t>(0x7f000001));

    EXPECT_EQ("127.0.0.1:80", inet_address((::sockaddr*)&v4).str());
    EXPECT_EQ("127.0.0.1:443", inet_address((::sockaddr*)&v4, 443).str());
    EXPECT_EQ("127.0.0.1:443", inet_address((::sockaddr*)&v4, "https").str());

    p6->sin6_family = AF_INET6;
    p6->sin6_port   = chen::num::swap(static_cast<std::uint16_t>(80));

    ip_version6 i6("fe80::1");
    ::memcpy(p6->sin6_addr.s6_addr, i6.addr().data(), 16);

    EXPECT_EQ("[fe80::1]:80", inet_address((::sockaddr*)&v6).str());
    EXPECT_EQ("[fe80::1]:443", inet_address((::sockaddr*)&v6, 443).str());
    EXPECT_EQ("[fe80::1]:443", inet_address((::sockaddr*)&v6, "https").str());

    // conversion
    auto c  = inet_address().sockaddr();
    auto c4 = inet_address((::sockaddr*)&v4).sockaddr();
    auto c6 = inet_address((::sockaddr*)&v6).sockaddr();

    EXPECT_TRUE(c.ss_family == 0);
    EXPECT_TRUE(::memcmp(&v4, &c4, sizeof(::sockaddr_in)) == 0);
    EXPECT_TRUE(::memcmp(&v6, &c6, sizeof(::sockaddr_in6)) == 0);
}

TEST(InetAddressTest, Assignment)
{
    inet_address addr;

    // normal
    addr.assign(nullptr);
    EXPECT_EQ("", addr.str());

    addr.assign("127.0.0.1", 80);
    EXPECT_EQ("127.0.0.1:80", addr.str());

    addr.assign("127.0.0.1", "http");
    EXPECT_EQ("127.0.0.1:80", addr.str());

    // operator
    ::sockaddr_storage empty{};  // family is 0
    EXPECT_ANY_THROW(addr.assign((::sockaddr*)&empty));

    addr = nullptr;
    EXPECT_EQ("", addr.str());

    addr = "[fe80::1]:80";
    EXPECT_EQ("[fe80::1]:80", addr.str());

    addr = std::string("[fe80::1]:80");
    EXPECT_EQ("[fe80::1]:80", addr.str());

    // raw address
    ::sockaddr_in *p4 = (::sockaddr_in*)&empty;

    p4->sin_family = AF_INET;
    p4->sin_port   = chen::num::swap(static_cast<std::uint16_t>(80));
    p4->sin_addr.s_addr = chen::num::swap(static_cast<std::uint32_t>(0x7f000001));

    addr = (::sockaddr*)&empty;
    EXPECT_EQ("127.0.0.1:80", addr.str());
}

TEST(InetAddressTest, Property)
{
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