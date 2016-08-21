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
#include <chen/tool/log.hpp>
#include <gtest/gtest.h>
#include <thread>

std::thread server_thread;

TEST(TcpSimpleTest, General)
{
    using chen::tcp::server;
    using chen::tcp::client;
    using chen::tcp::conn;

    // server callback
    auto conn_callback = [] (chen::tcp::conn conn) {

    };

    auto errc_callback = [] (chen::tcp::server *server, std::error_code code) {
        PILogE("tcp: error occur: %s, %s", server->local().str().c_str(), code.message().c_str());
    };

    // start the server, run in background
    server echo;
    server_thread = std::thread([&] () {
        echo.attach(conn_callback);
        echo.attach(errc_callback);
        echo.start("127.0.0.1", 8888);
    });

    // join the thread
    std::this_thread::sleep_for(std::chrono::seconds(1));

    echo.stop();
    server_thread.join();
}