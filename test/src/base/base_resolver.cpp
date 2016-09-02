/**
 * Created by Jian Chen
 * @since  2016.08.28
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/base_resolver.hpp>
#include <gtest/gtest.h>

TEST(BaseResolverTest, General)
{
    using chen::resolver;
    using chen::endpoint;

    // resolve domain
    EXPECT_NO_THROW(resolver::resolve("chensoft.com"));
    EXPECT_NO_THROW(resolver::resolve("chensoft.com:80"));
    EXPECT_NO_THROW(resolver::resolve("chensoft.com:http"));

    EXPECT_NO_THROW(resolver::async("chensoft.com").get());
    EXPECT_NO_THROW(resolver::async("chensoft.com:80").get());
    EXPECT_NO_THROW(resolver::async("chensoft.com:http").get());

    // resolve ipv4
    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::resolve(":80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::resolve("127.0.0.1"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1:80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::resolve("127.0.0.1:http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::resolve("", 80));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::resolve("127.0.0.1", 0));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::resolve("127.0.0.1", "80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::resolve("127.0.0.1", "http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::async(":80").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::async("127.0.0.1").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::async("127.0.0.1:80").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::async("127.0.0.1:http").get());

    EXPECT_EQ(std::vector<endpoint>{endpoint(":80")}, resolver::async("", 80).get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1")}, resolver::async("127.0.0.1", 0).get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:80")}, resolver::async("127.0.0.1", "80").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("127.0.0.1:http")}, resolver::async("127.0.0.1", "http").get());

    // resolve ipv6
    EXPECT_EQ(std::vector<endpoint>{endpoint("[::]:80")}, resolver::resolve("[::]:80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]")}, resolver::resolve("[fe80::1]"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::resolve("[fe80::1]:80"));
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:http")}, resolver::resolve("[fe80::1]:http"));

    EXPECT_EQ(std::vector<endpoint>{endpoint("[::]:80")}, resolver::async("[::]:80").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]")}, resolver::async("[fe80::1]").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:80")}, resolver::async("[fe80::1]:80").get());
    EXPECT_EQ(std::vector<endpoint>{endpoint("[fe80::1]:http")}, resolver::async("[fe80::1]:http").get());

    // service
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