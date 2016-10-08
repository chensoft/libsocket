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
             * Read data from remote, socket's callback will be called
             * todo use oneshot instead of edge trigger?
             */
            void read(net::socket *ptr, std::size_t size);

            /**
             * Write data to remote host, socket's callback will be called
             * todo use oneshot instead of edge trigger?
             */
            void write(net::socket *ptr, std::vector<std::uint8_t> &&data);

            /**
             * Remove all callbacks for specific socket
             */
            void remove(net::socket *ptr);

        public:
            /**
             * Poll events and dispatch
             */
            void start();

            /**
             * Stop proactor
             * @notice you can call this method in callback or other thread
             */
            void stop();

        private:
            proactor(const proactor&) = delete;
            proactor& operator=(const proactor&) = delete;

        private:
#ifndef _WIN32
            int _fd = -1;  // epoll on Linux, kqueue on Unix
            int _pp[2]{};  // use pipe to stop the event loop, 0 is read fd, 1 is write fd
#else
#endif

            typedef std::vector<std::uint8_t> chunk;

            // todo remove rename to read, write
            std::unordered_map<net::socket*, std::queue<chunk>> _read;   // read cache
            std::unordered_map<net::socket*, std::queue<chunk>> _write;  // write cache
        };
    }
}