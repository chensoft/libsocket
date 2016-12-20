/**
 * Created by Jian Chen
 * @since  2016.12.20
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_address.hpp>
#include <socket/core/reactor.hpp>
#include <gtest/gtest.h>

using chen::reactor;
using chen::basic_socket;
using chen::inet_address;

TEST(CoreReactorTest, LT)
{
    // create server
    basic_socket server(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(!server.bind(inet_address("127.0.0.1:0")));  // bind on a random port
    EXPECT_TRUE(!server.listen());

    // monitor read
    reactor r;
    r.set(server.native(), &server, reactor::ModeRead, 0);  // level-triggered

    // create client
    basic_socket client(AF_INET, SOCK_STREAM, 0);
    EXPECT_TRUE(!client.connect(server.sock()));

    // reactor should report readable event
    auto list = r.poll(1);

    EXPECT_EQ(1u, list.size());
    EXPECT_EQ(&server, list.front().data);
    EXPECT_EQ(reactor::Type::Readable, list.front().type);

    // poll should still report last event because we didn't accept the connection
    list = r.poll(1);

    EXPECT_EQ(1u, list.size());
    EXPECT_EQ(&server, list.front().data);
    EXPECT_EQ(reactor::Type::Readable, list.front().type);

    // accept connection
    EXPECT_TRUE(server.accept());

    // poll should return empty list because we have accepted the connection
    list = r.poll(1, 0.);  // we want return immediately

    EXPECT_TRUE(list.empty());
}

TEST(CoreReactorTest, ET)
{
    // create server
    basic_socket server(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(!server.bind(inet_address("127.0.0.1:0")));  // bind on a random port
    EXPECT_TRUE(!server.listen());

    // monitor read
    reactor r;
    r.set(server.native(), &server, reactor::ModeRead, reactor::FlagEdge);  // edge-triggered

    // create client
    basic_socket client(AF_INET, SOCK_STREAM, 0);
    EXPECT_TRUE(!client.connect(server.sock()));

    // reactor should report readable event
    auto list = r.poll(1);

    EXPECT_EQ(1u, list.size());
    EXPECT_EQ(&server, list.front().data);
    EXPECT_EQ(reactor::Type::Readable, list.front().type);

    // poll should timeout because it's edge-triggered, event won't occur twice
    list = r.poll(1, 0.);  // we want return immediately

    EXPECT_TRUE(list.empty());
}

TEST(CoreReactorTest, Once)
{
    // create server
    basic_socket server(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(!server.bind(inet_address("127.0.0.1:0")));  // bind on a random port
    EXPECT_TRUE(!server.listen());

    // monitor read
    reactor r;
    r.set(server.native(), &server, reactor::ModeRead, reactor::FlagOnce);  // report once

    // create client
    basic_socket client(AF_INET, SOCK_STREAM, 0);
    EXPECT_TRUE(!client.connect(server.sock()));

    // reactor should report readable event
    auto list = r.poll(1);

    EXPECT_EQ(1u, list.size());
    EXPECT_EQ(&server, list.front().data);
    EXPECT_EQ(reactor::Type::Readable, list.front().type);

    // poll should timeout because the event will report only once
    list = r.poll(1, 0.);  // we want return immediately

    EXPECT_TRUE(list.empty());
}

TEST(CoreReactorTest, Delete)
{
    // create server
    basic_socket server(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(!server.bind(inet_address("127.0.0.1:0")));  // bind on a random port
    EXPECT_TRUE(!server.listen());

    // monitor read
    reactor r;
    r.set(server.native(), &server, reactor::ModeRead, 0);  // level-triggered

    // create client
    basic_socket client(AF_INET, SOCK_STREAM, 0);
    EXPECT_TRUE(!client.connect(server.sock()));

    // reactor should report readable event
    auto list = r.poll(1);

    EXPECT_EQ(1u, list.size());

    // delete the handle
    r.del(server.native());

    // poll should timeout because no handle exist
    list = r.poll(1, 0.);  // we want return immediately

    EXPECT_TRUE(list.empty());
}