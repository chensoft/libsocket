/**
 * Created by Jian Chen
 * @since  2016.12.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/basic_socket.hpp>
#include <socket/inet/inet_address.hpp>
#include <socket/core/reactor.hpp>
#include <chen/mt/threadpool.hpp>
#include <gtest/gtest.h>

using chen::reactor;
using chen::inet_address;
using chen::basic_socket;

void server(basic_socket &s)
{
    using std::placeholders::_1;

    std::vector<basic_socket> cache;
    reactor r;

    auto handler_c = [&] (std::size_t index, reactor::Type type) {
        auto &socket = cache[index];

        // user should read rest of the data even type contains Closed
        EXPECT_TRUE((type & reactor::Readable) || (type & reactor::Closed));

        // unregister it
        if (type & reactor::Closed)
            r.del(socket.native());

        // read data from client
        auto size = socket.available();
        EXPECT_GE(size, 0u);

        if (!size)
            return;  // connection closed

        std::string text(size, '\0');
        EXPECT_EQ((chen::ssize_t)size, socket.recv(&text[0], size));

        // need stop the reactor?
        if (text == "stop")
            return r.stop();

        // revert and send back
        std::reverse(text.begin(), text.end());
        EXPECT_EQ((chen::ssize_t)size, socket.send(text.data(), size));
    };

    auto handler_s = [&] (reactor::Type type) {
        EXPECT_GT(type & reactor::Readable, 0);

        // accept new connection
        auto conn = s.accept();
        EXPECT_TRUE(conn);

        cache.emplace_back(std::move(conn));  // prevent connection released

        // register event for conn
        r.set(cache.back().native(), std::bind(handler_c, cache.size() - 1, _1), reactor::ModeRead, 0);
    };

    r.set(s.native(), handler_s, reactor::ModeRead, 0);

    r.run();
}

void client(std::uint16_t port)
{
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
        pool->post([&data, port, i] () {
            auto text = data[i % data.size()];

            basic_socket c(AF_INET, SOCK_STREAM, 0);

            EXPECT_TRUE(!c.connect(inet_address("127.0.0.1", port)));
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
    basic_socket c(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(!c.connect(inet_address("127.0.0.1", port)));
    EXPECT_EQ(4, c.send("stop", 4));
}

TEST(CoreReactorTest, Echo)
{
    // server
    basic_socket s(AF_INET, SOCK_STREAM, 0);

    EXPECT_TRUE(!s.bind(inet_address("127.0.0.1:0")));  // bind on a random port
    EXPECT_TRUE(!s.listen());

    std::thread t_server(std::bind(&server, std::ref(s)));

    // client
    std::thread t_client(std::bind(&client, inet_address(s.sock()).port()));

    t_server.join();
    t_client.join();
}