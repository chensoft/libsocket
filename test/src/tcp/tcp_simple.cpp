/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_client.hpp>
#include <socket/tcp/tcp_server.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_conn.hpp>
#include <gtest/gtest.h>
#include <thread>

std::thread server_thread;

TEST(TcpSimpleTest, General)
{
    using chen::tcp::server;
    using chen::tcp::client;
    using chen::tcp::conn;

    // start the server
    server_thread = std::thread([] () {
        server echo;
    });







    // join the thread
    server_thread.join();
}