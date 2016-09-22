/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>
#include <chen/tool/log.hpp>
#include <gtest/gtest.h>

TEST(TCPClientTest, Test)
{
    using chen::tcp::client;
    using chen::tcp::connecting_event;
    using chen::tcp::connected_event;
    using chen::tcp::disconnect_event;
    using chen::net::proactor;

    proactor p;

    PILogE("size: %zu, %zu, %zu", alignof(std::function<void ()>), sizeof(std::function<void ()>), sizeof(int));

    std::vector<std::unique_ptr<client>> list;

    for (int i = 0, len = 10; i < len; ++i)
    {
        list.push_back(std::unique_ptr<client>(new client(chen::ip::address::Type::IPv4, p)));

        client &c = *list.back();

        c.attach([i] (client &x, connecting_event &e) {
            PILogE("[%d] connecting: %s", i, e.ep.str().c_str());
        });

        c.attach([i] (client &x, connected_event &e) {
            PILogE("[%d] connected: %s, %s", i, e.ep.str().c_str(), e.err.message().c_str());
        });

        c.connect("223.202.26.4:80");
    }

    p.start();
}