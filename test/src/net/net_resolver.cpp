/**
 * Created by Jian Chen
 * @since  2016.09.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <gtest/gtest.h>

TEST(NetResolverTest, Domain)
{
    using chen::net::resolver;

    // resolve domain
    EXPECT_NO_THROW(resolver::resolve("example.com"));
    EXPECT_NO_THROW(resolver::resolve("example.com:80"));
    EXPECT_NO_THROW(resolver::resolve("example.com:http"));
}

TEST(NetResolverTest, IPv4)
{
    using chen::net::resolver;
    using chen::ip::address;

    // mixed, with address type
    EXPECT_EQ("0.0.0.0:80", resolver::resolve(":80")[0].str());
    EXPECT_EQ("127.0.0.1:0", resolver::resolve("127.0.0.1")[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1:80")[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1:http")[0].str());

    EXPECT_EQ("0.0.0.0:80", resolver::resolve(":80", address::Type::IPv4)[0].str());
    EXPECT_EQ("127.0.0.1:0", resolver::resolve("127.0.0.1", address::Type::IPv4)[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1:80", address::Type::IPv4)[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1:http", address::Type::IPv4)[0].str());

    // host, service, with address type
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1", "80")[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1", "http")[0].str());

    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1", "80", address::Type::IPv4)[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1", "http", address::Type::IPv4)[0].str());

    // host, port, with address type
    EXPECT_EQ("0.0.0.0:80", resolver::resolve("", 80)[0].str());
    EXPECT_EQ("127.0.0.1:0", resolver::resolve("127.0.0.1", 0)[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1", 80)[0].str());

    EXPECT_EQ("0.0.0.0:80", resolver::resolve("", 80, address::Type::IPv4)[0].str());
    EXPECT_EQ("127.0.0.1:0", resolver::resolve("127.0.0.1", 0, address::Type::IPv4)[0].str());
    EXPECT_EQ("127.0.0.1:80", resolver::resolve("127.0.0.1", 80, address::Type::IPv4)[0].str());
}

TEST(NetResolverTest, IPv6)
{
    using chen::net::resolver;
    using chen::ip::address;

    // mixed, with address type
    EXPECT_EQ("[::]:80", resolver::resolve("[::]:80")[0].str());
    EXPECT_EQ("[fe80::1]:0", resolver::resolve("[fe80::1]")[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("[fe80::1]:80")[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("[fe80::1]:http")[0].str());

    EXPECT_EQ("[::]:80", resolver::resolve("[::]:80", address::Type::IPv6)[0].str());
    EXPECT_EQ("[fe80::1]:0", resolver::resolve("[fe80::1]", address::Type::IPv6)[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("[fe80::1]:80", address::Type::IPv6)[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("[fe80::1]:http", address::Type::IPv6)[0].str());

    // host, service, with address type
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("fe80::1", "80")[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("fe80::1", "http")[0].str());

    EXPECT_EQ("[fe80::1]:80", resolver::resolve("fe80::1", "80", address::Type::IPv6)[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("fe80::1", "http", address::Type::IPv6)[0].str());

    // host, port, with address type
    EXPECT_EQ("[::]:80", resolver::resolve("::", 80)[0].str());
    EXPECT_EQ("[fe80::1]:0", resolver::resolve("fe80::1", 0)[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("fe80::1", 80)[0].str());

    EXPECT_EQ("[::]:80", resolver::resolve("::", 80, address::Type::IPv6)[0].str());
    EXPECT_EQ("[fe80::1]:0", resolver::resolve("fe80::1", 0, address::Type::IPv6)[0].str());
    EXPECT_EQ("[fe80::1]:80", resolver::resolve("fe80::1", 80, address::Type::IPv6)[0].str());
}

TEST(NetResolverTest, Reverse)
{
    using chen::net::resolver;

    // reverse resolve
    EXPECT_EQ(std::make_pair(std::string("localhost"), std::string("http")), resolver::reverse("127.0.0.1:80"));
    EXPECT_EQ(std::make_pair(std::string("localhost"), std::string("https")), resolver::reverse("127.0.0.1:443"));
}

TEST(NetResolverTest, Service)
{
    using chen::net::resolver;

    // service port and name
    EXPECT_EQ(0, resolver::service(""));

    EXPECT_EQ(80, resolver::service("80"));
    EXPECT_EQ(80, resolver::service("http"));

    EXPECT_EQ(80, resolver::service("80", "tcp"));
    EXPECT_EQ(80, resolver::service("http", "tcp"));

    EXPECT_EQ(53, resolver::service("domain", "tcp"));
    EXPECT_EQ(53, resolver::service("domain", "udp"));

    EXPECT_EQ("http", resolver::service(80));
    EXPECT_EQ("https", resolver::service(443));
}