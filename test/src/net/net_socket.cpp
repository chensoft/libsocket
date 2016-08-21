/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_socket.hpp>
#include <gtest/gtest.h>

TEST(NetSocketTest, Create)
{
    using chen::net::socket;

    // create socket
    EXPECT_NO_THROW(socket(socket::Family::IPv4, socket::Protocol::TCP));
    EXPECT_NO_THROW(socket(socket::Family::IPv4, socket::Protocol::UDP));

    EXPECT_NO_THROW(socket(socket::Family::IPv6, socket::Protocol::TCP));
    EXPECT_NO_THROW(socket(socket::Family::IPv6, socket::Protocol::UDP));

    EXPECT_NO_THROW(socket(socket::Family::Unix, socket::Protocol::TCP));
    EXPECT_NO_THROW(socket(socket::Family::Unix, socket::Protocol::UDP));

    EXPECT_EQ(socket::Family::IPv4, socket(socket::Family::IPv4, socket::Protocol::TCP).family());
    EXPECT_EQ(socket::Family::IPv6, socket(socket::Family::IPv6, socket::Protocol::TCP).family());
    EXPECT_EQ(socket::Family::Unix, socket(socket::Family::Unix, socket::Protocol::TCP).family());

    EXPECT_EQ(socket::Protocol::TCP, socket(socket::Family::IPv4, socket::Protocol::TCP).protocol());
    EXPECT_EQ(socket::Protocol::UDP, socket(socket::Family::IPv4, socket::Protocol::UDP).protocol());

    socket sock(socket::Family::IPv4, socket::Protocol::TCP);
    EXPECT_TRUE(sock.valid());

    sock.reset();
    EXPECT_TRUE(sock.valid());

    sock.close();
    EXPECT_FALSE(sock.valid());

    try
    {
        socket(socket::Family::IPv4, socket::Protocol::RAW);
        socket(socket::Family::IPv6, socket::Protocol::RAW);
    }
    catch (...)
    {
        // raw socket is not permit if you are not root user
    }
}