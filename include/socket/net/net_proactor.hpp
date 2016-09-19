/**
 * Created by Jian Chen
 * @since  2016.09.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>
#include <unordered_map>
#include <system_error>
#include <vector>

namespace chen
{
    namespace net
    {
        class proactor
        {
        public:
            proactor() throw(std::system_error);
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
             * Wait events and dispatch
             */
            void loop() throw(std::system_error);

        private:
            /**
             * Helper methods
             */
            void write(net::socket *ptr) throw(std::system_error);
            void read(net::socket *ptr) throw(std::system_error);

        private:
            proactor(const proactor&) = delete;
            proactor& operator=(const proactor&) = delete;

        private:
            notifier_t _fd = notifier_t();  // epoll on Linux, kqueue on Unix, IOCP on Windows

            typedef std::vector<std::uint8_t> chunk;

            std::unordered_map<net::socket*, std::vector<chunk>> _send;  // send cache
            std::unordered_map<net::socket*, std::vector<chunk>> _recv;  // recv cache
        };
    }
}