/**
 * Created by Jian Chen
 * @since  2016.09.18
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_message.hpp>
#include <socket/net/net_socket.hpp>
#include <socket/bsd/bsd_kqueue.hpp>
#include <socket/bsd/bsd_epoll.hpp>
#include <socket/bsd/bsd_poller.hpp>
#include <unordered_map>
#include <unordered_set>
#include <queue>

namespace chen
{
    namespace net
    {
        class notifier
        {
        public:
            notifier() = default;

        public:
            /**
             * Accept new connections, socket's onAccept method will be invoked when a new connection arrived
             * @attention you only need to call this method once
             */
            void accept(net::socket *ptr);

            /**
             * Read data from remote, socket's onRead method will be invoked when completed
             */
            void read(net::socket *ptr, std::size_t size);

            /**
             * Write data to remote in stream or datagram mode
             * socket's onWrite method will be invoked when all data is sent
             */
            void write(net::socket *ptr, std::vector<std::uint8_t> &&data);
            void write(net::socket *ptr, const std::vector<std::uint8_t> &data);

            void write(net::socket *ptr, std::vector<std::uint8_t> &&data, const bsd::endpoint &ep);
            void write(net::socket *ptr, const std::vector<std::uint8_t> &data, const bsd::endpoint &ep);

            /**
             * Remove all pending operations for specific socket
             * @attention socket need to remove itself when destruct
             */
            void remove(net::socket *ptr);

        public:
            /**
             * Poll events and dispatch
             */
            void start();

            /**
             * Stop notifier
             * @attention you can call this method in callback or other thread
             */
            void stop();

        private:
            /**
             * Internal helper
             */


        private:
            notifier(const notifier&) = delete;
            notifier& operator=(const notifier&) = delete;

        private:
#ifndef _WIN32
            bsd::reactor _model;
#else
            bsd::proactor _model;
#endif

            std::unordered_map<socket_t, net::socket*> _map;

            std::unordered_set<net::socket*> _accept;
            std::unordered_map<net::socket*, std::queue<net::message>> _read;
            std::unordered_map<net::socket*, std::queue<net::message>> _write;
        };
    }
}