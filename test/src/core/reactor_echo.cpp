/**
 * Created by Jian Chen
 * @since  2016.12.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/inet/inet_address.hpp>
#include <socket/core/reactor.hpp>
#include <chen/mt/threadpool.hpp>
#include <gtest/gtest.h>

using chen::reactor;
using chen::basic_event;
using chen::basic_socket;
using chen::basic_address;

void server_thread(basic_socket &s);
void client_thread(basic_address a);

TEST(CoreReactorTest, Echo)
{
    // server
    basic_socket s(AF_INET, SOCK_STREAM);

    EXPECT_TRUE(!s.bind(chen::inet_address("127.0.0.1:0")));  // bind on a random port
    EXPECT_TRUE(!s.listen());

    std::thread t_server(std::bind(&server_thread, std::ref(s)));

    // client
    std::thread t_client(std::bind(&client_thread, s.sock()));

    t_server.join();
    t_client.join();
}

void server_thread(basic_socket &s)
{
    using namespace std::placeholders;

    std::vector<std::unique_ptr<basic_socket>> cache;
    reactor r;

    auto handler_connection = [&](basic_socket *conn, int type) {
        // you should read the rest of the data even if you received the closed event
        EXPECT_TRUE((type & basic_event::Readable) || (type & basic_event::Closed));

        // read data from client
        auto size = conn->available();
        EXPECT_GE(size, 0u);

        if (!size)
            return;  // connection closed

        std::string text(size, '\0');
        EXPECT_EQ((chen::ssize_t)size, conn->recv(&text[0], size));

        // need stop the reactor?
        if (text == "stop")
            return r.stop();

        // revert and send back
        std::reverse(text.begin(), text.end());
        EXPECT_EQ((chen::ssize_t)size, conn->send(text.data(), size));
    };

    auto handler_server = [&](int type) {
        EXPECT_GT(type & basic_event::Readable, 0);

        // accept new connection
        std::unique_ptr<basic_socket> conn(new basic_socket);
        EXPECT_TRUE(!s.accept(*conn));

        conn->bind(std::bind(handler_connection, conn.get(), _1));

        cache.emplace_back(std::move(conn));  // prevent connection released

        // register event for conn
        r.set(cache.back().get(), reactor::ModeRead, 0);
    };

    s.bind(handler_server);
    r.set(&s, reactor::ModeRead, 0);

    r.run();
}

void client_thread(basic_address a)
{
    // send each message to server, server
    // will invert the string and send back
    std::vector<std::string> data = {
            "You say that you love rain",
            "but you open your umbrella when it rains",
            "You say that you love the sun",
            "but you find a shadow spot when the sun shines",
            "You say that you love the wind",
            "But you close your windows when wind blows",
            "This is why I am afraid",
            "You say that you love me too",
    };

    // spawn many clients to exchange data with server
    std::unique_ptr<chen::threadpool> pool(new chen::threadpool);

    for (std::size_t i = 0, l = data.size(); i < l; ++i)
    {
        pool->post([&data, a, i] () {
            auto text = data[i % data.size()];

            basic_socket c(AF_INET, SOCK_STREAM);

            EXPECT_TRUE(!c.connect(a));
            EXPECT_EQ((chen::ssize_t)text.size(), c.send(text.data(), text.size()));

            std::string response(text.size(), '\0');
            std::string reverse(text.rbegin(), text.rend());

            EXPECT_EQ((chen::ssize_t)text.size(), c.recv(&response[0], text.size()));
            EXPECT_EQ(reverse, response);  // the server will inverts the string
        });
    }

    // destroy the thread pool
    pool.reset();

    // tell the server to quit
    basic_socket c(AF_INET, SOCK_STREAM);

    EXPECT_TRUE(!c.connect(a));
    EXPECT_EQ(4, c.send("stop", 4));
}