/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "test_base.hpp"
#include <chen/net/udp/udp_client.hpp>
#include <chen/net/udp/udp_server.hpp>
#include <cstring>
#include <thread>

namespace chen
{
    class test_udp : public test_base
    {
    public:
        virtual void check() override
        {
            // test echo server
            this->check_echo();
        }

        virtual void check_echo()
        {
            // todo @@ can't stop server from another place now
            PILogD("unit udp: test echo start...");

            auto block = [&] () {
                PILogD("unit udp: test echo wait 2 seconds start");

                std::this_thread::sleep_for(std::chrono::seconds(2));

                PILogD("unit udp: test echo wait 2 seconds finish");

                udp::client client;
                client.send((const std::uint8_t *)"chen", 4, "127.0.0.1", 8888);

                std::uint8_t data[5] = {0};
                std::size_t size = 4;
                std::string addr;
                std::uint16_t port = 0;

                client.recv(data, size, addr, port);

                PILogD("unit udp: client recv: %s, %u, %s, %d", (char*)data, size, addr.c_str(), port);

                this->assert(::strcmp((char*)data, "chen") == 0, "unit udp: client recv data error");
                this->assert(size == 4, "unit udp: client recv size error");
                this->assert(addr == "127.0.0.1", "unit udp: client recv addr error");
                this->assert(port == 8888, "unit udp: client recv port error");
            };

            std::thread thread(std::bind(block));

            udp::server server;
            server.bind("0.0.0.0", 8888);
            server.attach([&server] (std::vector<std::uint8_t> data, std::string addr, std::uint16_t port) {
                server.send(data.data(), data.size(), addr, port);
                server.stop();
            });
            server.start();

            thread.join();

            PILogD("unit udp: test echo success\n");
        }

        virtual void bench() override
        {

        }
    };
}
