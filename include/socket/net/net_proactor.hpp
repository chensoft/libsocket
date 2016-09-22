/**
 * Created by Jian Chen
 * @since  2016.09.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>
#include <unordered_map>
#include <vector>
#include <queue>

namespace chen
{
    namespace net
    {
        class proactor
        {
        public:
            proactor();
            ~proactor();

        public:
            /**
             * Send data to remote host, socket's callback will be called
             */
            void send(net::socket *ptr, std::vector<std::uint8_t> &&data);

            /**
             * Receive data from remote, socket's callback will be called
             */
            void recv(net::socket *ptr, std::size_t size);

            /**
             * Remove all callbacks for specific socket
             */
            void remove(net::socket *ptr);

            /**
             * Wait events and dispatch
             */
            void start();

            /**
             * Stop proactor
             * you can call this method in callback or other thread
             */
            void stop();

        private:
            /**
             * Hepler methods
             */
            void write(net::socket *ptr);
            void read(net::socket *ptr);

        private:
            proactor(const proactor&) = delete;
            proactor& operator=(const proactor&) = delete;

        private:
#ifndef _WIN32
            int _fd = -1;  // epoll on Linux, kqueue on Unix
#else
            // todo use IOCP
#endif

            typedef std::vector<std::uint8_t> chunk;

            std::unordered_map<net::socket*, std::queue<chunk>> _send;  // send cache
            std::unordered_map<net::socket*, std::queue<chunk>> _recv;  // recv cache
        };
    }
}