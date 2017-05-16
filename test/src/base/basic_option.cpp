/**
 * Created by Jian Chen
 * @since  2016.11.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "socket/base/basic_socket.hpp"
#include "gtest/gtest.h"

using chen::basic_socket;
using chen::basic_option;
using chen::basic_address;

TEST(BasicOptionTest, TCP)
{
    basic_socket s(AF_INET, SOCK_STREAM);

    // error
    EXPECT_TRUE(!basic_option::error(s.native()));

    // type
    EXPECT_EQ(s.type(), basic_option::type(s.native()));

    // debug
    EXPECT_FALSE(basic_option::debug(s.native()));
    EXPECT_NO_THROW(basic_option::debug(s.native(), true));  // system may not allowed
    EXPECT_NO_THROW(basic_option::debug(s.native()));

    // reuseaddr
    EXPECT_FALSE(basic_option::reuseaddr(s.native()));
    EXPECT_TRUE(basic_option::reuseaddr(s.native(), true));
    EXPECT_TRUE(basic_option::reuseaddr(s.native()));

    // reuseport
#ifndef _WIN32
    EXPECT_FALSE(basic_option::reuseport(s.native()));
    EXPECT_TRUE(basic_option::reuseport(s.native(), true));
    EXPECT_TRUE(basic_option::reuseport(s.native()));
#endif

    // keepalive
    EXPECT_FALSE(basic_option::keepalive(s.native()));
    EXPECT_TRUE(basic_option::keepalive(s.native(), true));
    EXPECT_TRUE(basic_option::keepalive(s.native()));

    // dontroute
    EXPECT_FALSE(basic_option::dontroute(s.native()));
    EXPECT_TRUE(basic_option::dontroute(s.native(), true));
    EXPECT_TRUE(basic_option::dontroute(s.native()));

    // linger
    EXPECT_TRUE(basic_option::linger(s.native(), 1, 100));
    EXPECT_NO_THROW(basic_option::linger(s.native()));  // may not allowed

    // oobinline
    EXPECT_FALSE(basic_option::oobinline(s.native()));
    EXPECT_TRUE(basic_option::oobinline(s.native(), true));
    EXPECT_TRUE(basic_option::oobinline(s.native()));

    // sndbuf
    EXPECT_GT(basic_option::sndbuf(s.native()), 0);
    EXPECT_TRUE(basic_option::sndbuf(s.native(), 1024));  // just a hint

    // rcvbuf
    EXPECT_GT(basic_option::rcvbuf(s.native()), 0);
    EXPECT_TRUE(basic_option::rcvbuf(s.native(), 1024));  // just a hint

    // sndlowat
    EXPECT_GE(basic_option::sndlowat(s.native()), 0);
    EXPECT_NO_THROW(basic_option::sndlowat(s.native(), 1024));  // may not allowed

    // rcvlowat
    EXPECT_GE(basic_option::rcvlowat(s.native()), 0);
    EXPECT_NO_THROW(basic_option::rcvlowat(s.native(), 1024));  // may not allowed

    // sndtimeo
    EXPECT_TRUE(basic_option::sndtimeo(s.native(), 100, 4000));  // just a hint
    EXPECT_NO_THROW(basic_option::sndtimeo(s.native()));

    // rcvtimeo
    EXPECT_TRUE(basic_option::rcvtimeo(s.native(), 100, 4000));  // just a hint
    EXPECT_NO_THROW(basic_option::rcvtimeo(s.native()));
}

TEST(BasicOptionTest, UDP)
{
    basic_socket s(AF_INET, SOCK_DGRAM);

    // broadcast
    EXPECT_FALSE(basic_option::broadcast(s.native()));
    EXPECT_TRUE(basic_option::broadcast(s.native(), true));
    EXPECT_TRUE(basic_option::broadcast(s.native()));
}