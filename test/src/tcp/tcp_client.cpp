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

    client c(chen::ip::address::Type::IPv6);

    c.attach([] (chen::tcp::client &t, chen::tcp::event::basic *ev) {
        switch (ev->type)
        {
            case chen::tcp::event::basic::Type::Connecting:
                printf("client: %d, %s\n", ev->type, static_cast<chen::tcp::event::connecting*>(ev)->ep.str().c_str());
                break;

            case chen::tcp::event::basic::Type::Connected:
                printf("client: %d, %s\n", ev->type, static_cast<chen::tcp::event::connected*>(ev)->err.message().c_str());
                break;

            default:
                printf("client: %d\n", ev->type);
                break;
        }
    });

    c.connect("93.184.216.34", 80);
}