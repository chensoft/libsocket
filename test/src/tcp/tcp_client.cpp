/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>
#include <chen/tool/log.hpp>
#include <gtest/gtest.h>
#include <thread>

// todo remove tcp_client test, add more general tcp test
TEST(TCPClientTest, Test)
{
    using chen::tcp::client;
    using chen::tcp::connecting_event;
    using chen::tcp::connected_event;
    using chen::tcp::disconnect_event;
    using chen::net::proactor;

    proactor p;

    std::thread t([&] () {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        p.stop();
    });

    PILogE("size: %zu, %zu, %zu", alignof(std::function<void ()>), sizeof(std::function<void ()>), sizeof(proactor));

    client c(chen::ip::address::Type::IPv4, p);

    c.attach([] (client &x, connecting_event &e) {
        PILogE("connecting: %s", e.ep.str().c_str());
    });

    c.attach([&] (client &x, connected_event &e) {
        PILogE("connected: %s, %s", e.ep.str().c_str(), e.err.message().c_str());
    });

    c.connect("example.com:80");

    p.start();
    t.join();
}