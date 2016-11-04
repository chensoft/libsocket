/**
 * Created by Jian Chen
 * @since  2016.11.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/prot/udp/udp_basic.hpp>

namespace chen
{
    namespace udp
    {
        class server : public basic
        {
        public:
            typedef std::function<void (std::vector<std::uint8_t> data)> callback_type;

        public:
            /**
             * Construct by the port, use IPv6 by default
             * @attention modern os support both v4 & v6 traffic if you use AF_INET6 to bind socket
             */
            server(net::runloop &runloop, std::uint16_t port, ip::address::Type type = ip::address::Type::IPv6);

            /**
             * Construct by the first resolved endpoint
             * the final endpoint type is determined by resolver
             * @attention throw exception if no dns record found or dns error
             */
            server(net::runloop &runloop, const char *mixed);
            server(net::runloop &runloop, const std::string &mixed, ip::address::Type type = ip::address::Type::None);
            server(net::runloop &runloop, const std::string &host, std::uint16_t port, ip::address::Type type = ip::address::Type::None);
            server(net::runloop &runloop, const std::string &host, const std::string &service, ip::address::Type type = ip::address::Type::None);

            /**
             * Construct by the endpoint
             */
            server(net::runloop &runloop, const net::endpoint &ep);

            ~server();

        public:
            /**
             * Run the server
             * :-) change the socket to nonblocking mode
             * :-) set reuse addr option for the socket
             * :-) bind the socket to specific endpoint
             * :-) listen and receive packet from remote
             */
            void run(callback_type callback);
            void run(int backlog, callback_type callback);

            /**
             * Stop this server
             */
            void stop();

        public:
            /**
             * Check state
             */
            bool isRunning();

            /**
             * Local endpoint
             */
            net::endpoint local() const;

        protected:
            /**
             * Listen and serve
             */
            void listen(int backlog);

            /**
             * Event handler
             */
            void onReadable();
            void onWritable();
            void onEnded();
            void onEvent(net::runloop::Event type);

        protected:
            bool _running = false;

            net::endpoint _local;
            net::runloop &_runloop;

            callback_type _callback;
        };
    }
}