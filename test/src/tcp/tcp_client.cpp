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
    client c = client::v4(p);

    PILogE("size: %zu, %zu, %zu", alignof(std::function<void ()>), sizeof(std::function<void ()>), sizeof(int));

    c.attach([] (client &x, connecting_event &e) {
        PILogE("connecting: %s", e.ep.str().c_str());
    });

    c.attach([] (client &x, connected_event &e) {
        PILogE("connected: %s, %s", e.ep.str().c_str(), e.err.message().c_str());

        PILogE("sock: %s, peer: %s, remote: %s", x.sock().str().c_str(), x.peer().str().c_str(), x.remote().str().c_str());
        x.disconnect();
        PILogE("sock: %s, peer: %s, remote: %s", x.sock().str().c_str(), x.peer().str().c_str(), x.remote().str().c_str());

        PILogE("non-blocking: %d, valid: %d, native: %d", x.nonblocking(), x.valid(), x.native());
        PILogE("option: %d", x.option().nodelay());
    });

    c.connect("139.196.204.109:80");
//    c.connect("127.0.0.1:80");

//    p.start();
}