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

    client c = client::v4();

    PILogE("local: %s, remote: %s", c.local().str().c_str(), c.remote().str().c_str());
    PILogE("non-blocking: %d, valid: %d, native: %d", c.nonblocking(), c.valid(), c.native());
    PILogE("option: %d", c.option().nodelay());

    c.attach([] (client &c, connecting_event &e) {
        PILogE("connecting: %s", e.ep.str().c_str());
    });

    c.connect("139.196.204.109:80");
}