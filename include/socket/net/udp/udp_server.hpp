/**
 * Created by Jian Chen
 * @since  2016.11.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/udp/udp_basic.hpp>

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
            server(runloop &runloop, std::uint16_t port, ip_address::Type type = ip_address::Type::IPv6);

            /**
             * Construct by the first resolved endpoint
             * the final endpoint type is determined by resolver
             * @attention throw exception if no dns record found or dns error
             */
            server(runloop &runloop, const char *mixed);
            server(runloop &runloop, const std::string &mixed, ip_address::Type type = ip_address::Type::None);
            server(runloop &runloop, const std::string &host, std::uint16_t port, ip_address::Type type = ip_address::Type::None);
            server(runloop &runloop, const std::string &host, const std::string &service, ip_address::Type type = ip_address::Type::None);

            /**
             * Construct by the endpoint
             */
            server(runloop &runloop, const inet_endpoint &ep);

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
            inet_endpoint local() const;

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
            void onEvent(runloop::Event type);

        protected:
            bool _running = false;

            inet_endpoint _local;
            runloop &_runloop;

            callback_type _callback;
        };
    }
}