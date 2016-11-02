/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_conn.hpp>

namespace chen
{
    namespace tcp
    {
        class server : public basic
        {
        public:
            // todo add param to accept init option
            explicit server(net::runloop &runloop);
            ~server();

        public:
            /**
             * Listening on the port, use IPv6 address type by default
             * @attention modern os support both v4 & v6 traffic if you bind socket by AF_INET6
             */
            void listen(std::uint16_t port, ip::address::Type type = ip::address::Type::IPv6);

            /**
             * Listening on the first resolved endpoint
             * @attention throw exception if no dns record found or dns error
             */
            void listen(const char *mixed);
            void listen(const std::string &mixed, ip::address::Type type = ip::address::Type::None);
            void listen(const std::string &host, std::uint16_t port, ip::address::Type type = ip::address::Type::None);
            void listen(const std::string &host, const std::string &service, ip::address::Type type = ip::address::Type::None);

            /**
             * Listening on the endpoint
             * @attention server will be nonblocking and SO_REUSEADDR is true
             */
            void listen(const net::endpoint &ep);
            void listen(const net::endpoint &ep, int backlog);

        public:
            /**
             * Stop this server and disconnect all connections
             */
            void stop();

            /**
             * Pause the server
             * new connections will be rejected
             * exist connections will continue to be served
             */
            void pause();

            /**
             * Resume the server
             */
            void resume();

        public:
            /**
             * Check state
             */
            bool isClosed();
            bool isListening();

            /**
             * Local endpoint
             */
            net::endpoint local() const;

        protected:
            /**
             * Event handler
             */
            void onReadable();
            void onWritable();
            void onEnded();
            void onEvent(net::runloop::Event type);

        protected:
        };
    }
}