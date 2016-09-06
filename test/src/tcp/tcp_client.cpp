/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>
#include <gtest/gtest.h>

TEST(TCPClientTest, Fetch)
{
    using chen::tcp::client;

    client c;

    c.attach([] (chen::tcp::client &t, chen::tcp::event::basic *ev) {
        printf("xxx\n");
    });

    c.connect("93.184.216.34", 80);
}