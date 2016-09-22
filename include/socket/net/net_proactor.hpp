/**
 * Created by Jian Chen
 * @since  2016.09.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>
#include <vector>

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
            proactor(const proactor&) = delete;
            proactor& operator=(const proactor&) = delete;

        private:
            class proactor_detail;
            proactor_detail *_detail;
        };
    }
}