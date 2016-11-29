/**
 * Created by Jian Chen
 * @since  2016.11.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_socket.hpp>
#include <socket/inet/inet_address.hpp>
#include <chen/mt/semaphore.hpp>
#include <chen/base/num.hpp>
#include <gtest/gtest.h>
#include <thread>

using chen::basic_socket;
using chen::inet_address;
using chen::basic_address;
using chen::invalid_handle;

TEST(BasicSocketTest, Empty)
{
    EXPECT_FALSE(basic_socket());
    EXPECT_FALSE(basic_socket().valid());

    EXPECT_FALSE(basic_socket(nullptr));
    EXPECT_FALSE(basic_socket(nullptr).valid());

    EXPECT_FALSE(basic_socket(invalid_handle));
    EXPECT_FALSE(basic_socket(invalid_handle, 0, 0, 0));

    EXPECT_ANY_THROW(basic_socket(AF_INET, SOCK_DGRAM, IPPROTO_TCP));  // wrong protocol
}

TEST(BasicSocketTest, Create)
{
    // create via family
    basic_socket s1(AF_INET, SOCK_DGRAM, 0);
    basic_socket s2(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(s1);
    EXPECT_TRUE(s2);
    EXPECT_TRUE(!s2.nonblocking(true));

    EXPECT_NE(s1.native(), s2.native());  // handle is unique

    // create via handle
    basic_socket s3(s1.detach());
    basic_socket s4(s2.detach(), s2.family(), s2.type(), s2.protocol());

    EXPECT_FALSE(s1);  // s1, s2 already detached
    EXPECT_FALSE(s2);

    EXPECT_TRUE(s3);
    EXPECT_TRUE(s4);

    EXPECT_ANY_THROW(s3.reset());  // because no family info
    EXPECT_NO_THROW(s4.reset());   // because family info is provided when construct

    // create via move
    EXPECT_FALSE(s3);  // s3 already closed in above code
    EXPECT_TRUE(s3 = basic_socket(std::move(s4)));

    EXPECT_TRUE(s3);
    EXPECT_FALSE(s4);
}

TEST(BasicSocketTest, TCP)
{
    chen::semaphore sem;
    std::uint16_t port = 0;

    std::thread thread_s([&] () {
        // a simple echo server
        basic_socket server(AF_INET, SOCK_STREAM, 0);

        EXPECT_TRUE(!server.bind(inet_address("127.0.0.1:0")));  // bind on a random port
        EXPECT_TRUE(!server.listen());
        EXPECT_GT(port = inet_address(server.sock()).port(), 0);  // retrieve random port number

        // notify client to connect
        sem.post();

        // wait for new connections
        while (true)
        {
            auto conn = server.accept();
            EXPECT_TRUE(conn);

            // remote address
            EXPECT_EQ("127.0.0.1", inet_address(conn.peer()).addr().str());

            // retrieve available bytes to read
            EXPECT_GE(conn.available(), 0u);

            // receive message
            char buff[512]{};
            auto size = conn.recv(buff, 511);

            EXPECT_GT(size, 0);
            EXPECT_EQ(size, conn.send(buff, size));

            // receive shutdown
            size = conn.recv(buff, 511);
            EXPECT_LE(size, 0);

            if (::strcmp(buff, "\n") == 0)
                break;
        }
    });

    std::thread thread_c([&] () {
        sem.wait();  // wait for server become ready

        inet_address addr("127.0.0.1", port);

        // a simple echo client
        for (int i = 0; i < 10; ++i)
        {
            basic_socket client(AF_INET, SOCK_STREAM, 0);

            std::string text("hello, " + chen::num::str(i));

            EXPECT_TRUE(!client.connect(addr));
            EXPECT_EQ(static_cast<chen::ssize_t>(text.size()), client.send(text.data(), text.size()));

            char buff[512]{};
            auto size = client.recv(buff, 511);

            EXPECT_EQ(text, buff);
            EXPECT_EQ(static_cast<chen::ssize_t>(text.size()), size);

            // the following code is just for demonstration purpose
            client.shutdown(basic_socket::Shutdown::Read);   // close read channel
            client.shutdown(basic_socket::Shutdown::Write);  // close write channel
            client.shutdown(basic_socket::Shutdown::Both);   // close read & write
            client.shutdown();  // the same to Shutdown::Both
        }

        // stop it
        basic_socket stop(AF_INET, SOCK_STREAM, 0);

        EXPECT_TRUE(!stop.connect(addr));
        EXPECT_EQ(1, stop.send("\n", 1));
    });

    thread_s.join();
    thread_c.join();
}

TEST(BasicSocketTest, UDP)
{
    chen::semaphore sem;
    std::uint16_t port = 0;

    std::thread thread_s([&] () {
        // a simple echo server
        basic_socket server(AF_INET, SOCK_DGRAM, 0);

        EXPECT_TRUE(!server.bind(inet_address("127.0.0.1:0")));  // bind on a random port
        EXPECT_GT(port = inet_address(server.sock()).port(), 0);  // retrieve random port number

        // notify client to send message
        sem.post();

        // wait for new messages to arrive
        while (true)
        {
            basic_address addr;

            char buff[512]{};
            auto size = server.recvfrom(buff, 511, addr);

            EXPECT_GT(size, 0);
            EXPECT_EQ(size, server.sendto(buff, size, addr));

            // remote address
            EXPECT_EQ("127.0.0.1", inet_address(addr).addr().str());

            if (::strcmp(buff, "\n") == 0)
                break;
        }
    });

    std::thread thread_c([&] () {
        sem.wait();  // wait for server become ready

        inet_address addr("127.0.0.1", port);

        // a simple echo client
        for (int i = 0; i < 10; ++i)
        {
            basic_socket client(AF_INET, SOCK_DGRAM, 0);

            std::string text("hello, " + chen::num::str(i));
            EXPECT_EQ(static_cast<chen::ssize_t>(text.size()), client.sendto(text.data(), text.size(), addr));

            basic_address tmp;
            char buff[512]{};
            auto size = client.recvfrom(buff, 511, tmp);

            EXPECT_EQ(text, buff);
            EXPECT_EQ(static_cast<chen::ssize_t>(text.size()), size);
        }

        // stop it
        basic_socket stop(AF_INET, SOCK_DGRAM, 0);
        EXPECT_EQ(1, stop.sendto("\n", 1, addr));
    });

    thread_s.join();
    thread_c.join();
}