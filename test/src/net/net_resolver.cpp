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
    using chen::net::endpoint;
    using chen::ip::address;

    // mixed, with address type
    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::resolve(":80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::resolve("127.0.0.1"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1:80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::resolve("127.0.0.1:http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::resolve(":80", address::Type::IPv4));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::resolve("127.0.0.1", address::Type::IPv4));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1:80", address::Type::IPv4));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::resolve("127.0.0.1:http", address::Type::IPv4));

    // host, service, with address type
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1", "80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::resolve("127.0.0.1", "http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1", "80", address::Type::IPv4));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::resolve("127.0.0.1", "http", address::Type::IPv4));

    // host, port, with address type
    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::resolve("", 80));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::resolve("127.0.0.1", 0));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1", 80));

    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::resolve("", 80, address::Type::IPv4));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::resolve("127.0.0.1", 0, address::Type::IPv4));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1", 80, address::Type::IPv4));
}

TEST(NetResolverTest, IPv6)
{
    using chen::net::resolver;
    using chen::net::endpoint;
    using chen::ip::address;

    // mixed, with address type
    EXPECT_EQ(std::vector<endpoint>{endpoint("[::]:80")}, resolver::resolve("[::]:80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]")}, resolver::resolve("[fe80::1]"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("[fe80::1]:80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:http")}, resolver::resolve("[fe80::1]:http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint("[::]:80")}, resolver::resolve("[::]:80", address::Type::IPv6));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]")}, resolver::resolve("[fe80::1]", address::Type::IPv6));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("[fe80::1]:80", address::Type::IPv6));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:http")}, resolver::resolve("[fe80::1]:http", address::Type::IPv6));

    // host, service, with address type
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("fe80::1", "80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:http")}, resolver::resolve("fe80::1", "http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("fe80::1", "80", address::Type::IPv6));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:http")}, resolver::resolve("fe80::1", "http", address::Type::IPv6));

    // host, port, with address type
    EXPECT_EQ(std::vector<endpoint>{endpoint("[::]:80")}, resolver::resolve("::", 80));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]")}, resolver::resolve("fe80::1", 0));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("fe80::1", 80));

    EXPECT_EQ(std::vector<endpoint>{endpoint("[::]:80")}, resolver::resolve("::", 80, address::Type::IPv6));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]")}, resolver::resolve("fe80::1", 0, address::Type::IPv6));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("fe80::1", 80, address::Type::IPv6));
}

TEST(NetResolverTest, First)
{
    using chen::net::resolver;
    using chen::net::endpoint;
    using chen::ip::address;

    // mixed, with address type
    EXPECT_EQ(endpoint(":80"), resolver::first(":80"));
    EXPECT_EQ(endpoint("127.0.0.1"), resolver::first("127.0.0.1"));
    EXPECT_EQ(endpoint("127.0.0.1:80"), resolver::first("127.0.0.1:80"));
    EXPECT_EQ(endpoint("127.0.0.1:http"), resolver::first("127.0.0.1:http"));

    EXPECT_EQ(endpoint(":80"), resolver::first(":80", address::Type::IPv4));
    EXPECT_EQ(endpoint("127.0.0.1"), resolver::first("127.0.0.1", address::Type::IPv4));
    EXPECT_EQ(endpoint("127.0.0.1:80"), resolver::first("127.0.0.1:80", address::Type::IPv4));
    EXPECT_EQ(endpoint("127.0.0.1:http"), resolver::first("127.0.0.1:http", address::Type::IPv4));

    // host, service, with address type
    EXPECT_EQ(endpoint("127.0.0.1:80"), resolver::first("127.0.0.1", "80"));
    EXPECT_EQ(endpoint("127.0.0.1:http"), resolver::first("127.0.0.1", "http"));

    EXPECT_EQ(endpoint("127.0.0.1:80"), resolver::first("127.0.0.1", "80", address::Type::IPv4));
    EXPECT_EQ(endpoint("127.0.0.1:http"), resolver::first("127.0.0.1", "http", address::Type::IPv4));

    // host, port, with address type
    EXPECT_EQ(endpoint(":80"), resolver::first("", 80));
    EXPECT_EQ(endpoint("127.0.0.1"), resolver::first("127.0.0.1", 0));
    EXPECT_EQ(endpoint("127.0.0.1:80"), resolver::first("127.0.0.1", 80));

    EXPECT_EQ(endpoint(":80"), resolver::first("", 80, address::Type::IPv4));
    EXPECT_EQ(endpoint("127.0.0.1"), resolver::first("127.0.0.1", 0, address::Type::IPv4));
    EXPECT_EQ(endpoint("127.0.0.1:80"), resolver::first("127.0.0.1", 80, address::Type::IPv4));
}

TEST(NetResolverTest, Service)
{
    using chen::net::resolver;

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