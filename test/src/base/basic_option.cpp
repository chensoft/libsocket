/**
 * Created by Jian Chen
 * @since  2016.11.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_socket.hpp>
#include <socket/base/basic_option.hpp>
#include <gtest/gtest.h>

using chen::basic_socket;
using chen::basic_option;
using chen::basic_address;

TEST(BasicOptionTest, TCP)
{
    basic_socket s(AF_INET, SOCK_STREAM, 0);
    basic_option o = s.option();

    // error
    EXPECT_TRUE(!o.error());

    // type
    EXPECT_EQ(s.type(), o.type());

    // debug
    EXPECT_FALSE(o.debug());
    EXPECT_NO_THROW(o.debug(true));  // system may not allowed
    EXPECT_NO_THROW(o.debug());

    // reuseaddr
    EXPECT_FALSE(o.reuseaddr());
    EXPECT_TRUE(o.reuseaddr(true));
    EXPECT_TRUE(o.reuseaddr());

    // reuseport
#ifndef _WIN32
    EXPECT_FALSE(o.reuseport());
    EXPECT_TRUE(o.reuseport(true));
    EXPECT_TRUE(o.reuseport());
#endif

    // keepalive
    EXPECT_FALSE(o.keepalive());
    EXPECT_TRUE(o.keepalive(true));
    EXPECT_TRUE(o.keepalive());

    // dontroute
    EXPECT_FALSE(o.dontroute());
    EXPECT_TRUE(o.dontroute(true));
    EXPECT_TRUE(o.dontroute());

    // linger
    EXPECT_TRUE(o.linger(1, 100));

    auto time = o.linger();
    EXPECT_EQ(1, time.l_onoff);
    EXPECT_EQ(100, time.l_linger);

    // oobinline
    EXPECT_FALSE(o.oobinline());
    EXPECT_TRUE(o.oobinline(true));
    EXPECT_TRUE(o.oobinline());

    // sndbuf
    EXPECT_GT(o.sndbuf(), 0);
    EXPECT_TRUE(o.sndbuf(1024));  // just a hint

    // rcvbuf
    EXPECT_GT(o.rcvbuf(), 0);
    EXPECT_TRUE(o.rcvbuf(1024));  // just a hint

    // sndlowat
    EXPECT_GT(o.sndlowat(), 0);
    EXPECT_NO_THROW(o.sndlowat(1024));  // may not allowed

    // rcvlowat
    EXPECT_GT(o.rcvlowat(), 0);
    EXPECT_NO_THROW(o.rcvlowat(1024));  // may not allowed

    // sndtimeo
    EXPECT_TRUE(o.sndtimeo(100, 4000));  // just a hint
    EXPECT_NO_THROW(o.sndtimeo());

    // rcvtimeo
    EXPECT_TRUE(o.rcvtimeo(100, 4000));  // just a hint
    EXPECT_NO_THROW(o.rcvtimeo());
}

TEST(BasicOptionTest, UDP)
{
    basic_socket s(AF_INET, SOCK_DGRAM, 0);
    basic_option o = s.option();

    // broadcast
    EXPECT_FALSE(o.broadcast());
    EXPECT_TRUE(o.broadcast(true));
    EXPECT_TRUE(o.broadcast());
}