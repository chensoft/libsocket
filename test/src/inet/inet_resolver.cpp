/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/inet/inet_resolver.hpp"
#include "gtest/gtest.h"

using chen::ip_address;
using chen::inet_resolver;

TEST(InetResolverTest, Domain)
{
    // resolve domain
    EXPECT_NO_THROW(inet_resolver::resolve("example.com"));
    EXPECT_NO_THROW(inet_resolver::resolve("example.com:80"));
    EXPECT_NO_THROW(inet_resolver::resolve("example.com:http"));
}

TEST(InetResolverTest, IPv4)
{
    EXPECT_EQ("0.0.0.0:80", inet_resolver::resolve(":80")[0].str());
    EXPECT_EQ("127.0.0.1:0", inet_resolver::resolve("127.0.0.1")[0].str());
    EXPECT_EQ("127.0.0.1:80", inet_resolver::resolve("127.0.0.1:80")[0].str());
    EXPECT_EQ("127.0.0.1:80", inet_resolver::resolve("127.0.0.1:http")[0].str());

    EXPECT_EQ("0.0.0.0:80", inet_resolver::resolve(":80", AF_INET)[0].str());
    EXPECT_EQ("127.0.0.1:0", inet_resolver::resolve("127.0.0.1", AF_INET)[0].str());
    EXPECT_EQ("127.0.0.1:80", inet_resolver::resolve("127.0.0.1:80", AF_INET)[0].str());
    EXPECT_EQ("127.0.0.1:80", inet_resolver::resolve("127.0.0.1:http", AF_INET)[0].str());
}

TEST(InetResolverTest, IPv6)
{
    EXPECT_EQ("[fe80::1]:0", inet_resolver::resolve("fe80::1")[0].str());
    EXPECT_EQ("[fe80::1]:0", inet_resolver::resolve("[fe80::1]")[0].str());

    EXPECT_EQ("[::]:80", inet_resolver::resolve("[::]:80")[0].str());
    EXPECT_EQ("[fe80::1]:80", inet_resolver::resolve("[fe80::1]:80")[0].str());
    EXPECT_EQ("[fe80::1]:80", inet_resolver::resolve("[fe80::1]:http")[0].str());

    EXPECT_EQ("[::]:80", inet_resolver::resolve("[::]:80", AF_INET6)[0].str());
    EXPECT_EQ("[fe80::1]:80", inet_resolver::resolve("[fe80::1]:80", AF_INET6)[0].str());
    EXPECT_EQ("[fe80::1]:80", inet_resolver::resolve("[fe80::1]:http", AF_INET6)[0].str());
}

TEST(InetResolverTest, Reverse)
{
    // reverse resolve
	EXPECT_NO_THROW(inet_resolver::reverse("127.0.0.1:80"));
	EXPECT_NO_THROW(inet_resolver::reverse("127.0.0.1:443"));
}

TEST(InetResolverTest, Service)
{
    // service port and name
    EXPECT_EQ(0, inet_resolver::service(""));

    EXPECT_EQ(80, inet_resolver::service("80"));
    EXPECT_EQ(80, inet_resolver::service("http"));

    EXPECT_EQ(80, inet_resolver::service("80", "tcp"));
    EXPECT_EQ(80, inet_resolver::service("http", "tcp"));

    EXPECT_EQ(53, inet_resolver::service("domain", "tcp"));
    EXPECT_EQ(53, inet_resolver::service("domain", "udp"));

    EXPECT_EQ(513, inet_resolver::service("who"));  // only udp

    EXPECT_EQ("http", inet_resolver::service(80));
    EXPECT_EQ("https", inet_resolver::service(443));
}