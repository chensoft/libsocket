/**
 * Created by Jian Chen
 * @since  2016.09.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>
#include <socket/bsd/bsd_kqueue.hpp>
#include <socket/bsd/bsd_epoll.hpp>
#include <socket/bsd/bsd_iocp.hpp>

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
             * Read data from remote, socket's onRead will be invoked when completed
             */
            void read(net::socket *ptr, std::size_t size);

            /**
             * Write data to remote, socket's onWrite will be invoked when all data is sent
             */
            void write(net::socket *ptr, std::vector<std::uint8_t> &&data);
            void write(net::socket *ptr, const std::vector<std::uint8_t> &data);

            /**
             * Remove all pending operations for specific socket
             * @notice socket need to remove itself when destruct
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
#if !defined(__linux__) && !defined(_WIN32)
            bsd::kqueue _model;
#elif defined(__linux__)
            bsd::epoll _model;
#else
            bsd::iocp _model;
#endif
        };
    }
}