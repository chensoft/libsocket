/**
 * Created by Jian Chen
 * @since  2016.09.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#if !defined(__linux__) && !defined(_WIN32)

#include <socket/net/net_proactor.hpp>
#include <unordered_map>
#include <sys/event.h>
#include <queue>

namespace chen
{
    namespace net
    {
        class proactor::proactor_detail
        {
        public:
            proactor_detail();
            ~proactor_detail();

        public:
            void send(net::socket *ptr, std::vector<std::uint8_t> &&data);
            void recv(net::socket *ptr, std::size_t size);
            void remove(net::socket *ptr);
            void start();
            void stop();

        private:
            void write(net::socket *ptr);
            void read(net::socket *ptr);

        private:
            int _fd = -1;

            typedef std::vector<std::uint8_t> chunk;

            std::unordered_map<net::socket*, std::queue<chunk>> _send;  // send cache
            std::unordered_map<net::socket*, std::queue<chunk>> _recv;  // recv cache
        };
    }
}

#endif