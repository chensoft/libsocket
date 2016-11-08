/**
 * Created by Jian Chen
 * @since  2016.11.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_handler.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_conn.hpp>

namespace chen
{
    namespace tcp
    {
        class server : public basic
        {
        public:
            /**
             * Construct by the port, use IPv6 by default
             * @attention modern os support both v4 & v6 traffic if you use AF_INET6 to bind socket
             */
            server(runloop &runloop, std::uint16_t port, ip_address::Type type = ip_address::Type::IPv6);

            /**
             * Construct by the first resolved address
             * the final address type is determined by resolver
             * @attention throw exception if no dns record found or dns error
             */
            server(runloop &runloop, const char *mixed);
            server(runloop &runloop, const std::string &mixed, ip_address::Type type = ip_address::Type::None);
            server(runloop &runloop, const std::string &host, std::uint16_t port, ip_address::Type type = ip_address::Type::None);
            server(runloop &runloop, const std::string &host, const std::string &service, ip_address::Type type = ip_address::Type::None);

            /**
             * Construct by the address
             */
            server(runloop &runloop, const inet_address &addr);

            ~server();

        public:
            /**
             * Run the server
             * :-) change the socket to nonblocking mode
             * :-) set reuse addr option for the socket
             * :-) bind the socket to specific address
             * :-) listen and accept new connections
             * @attention the Handler must be a subclass of tcp::conn
             */
            // todo rename to listen & close
            template <typename Handler>
            void run(int backlog = 0)
            {
                this->_factory = [&] () {
                    return std::unique_ptr<handler>(new Handler);
                };
                this->listen(backlog);
            }

            /**
             * Stop this server and disconnect all connections
             */
            void stop();

        public:
            /**
             * Check state
             */
            bool isRunning();

            /**
             * Local address
             */
            inet_address local() const;

        protected:
            /**
             * Listen and serve
             */
            void listen(int backlog);

            /**
             * Server event
             */
            void onServerReadable();
            void onServerWritable();
            void onServerEnded();
            void onServerEvent(runloop::Event type);

            /**
             * Connection event
             */
            void onConnReadable(std::unique_ptr<conn> &c);
            void onConnWritable(std::unique_ptr<conn> &c);
            void onConnEnded(std::unique_ptr<conn> &c);
            void onConnEvent(std::unique_ptr<conn> &c, runloop::Event type);

        protected:
            bool _running = false;

            inet_address _local;
            runloop &_runloop;

            std::vector<std::unique_ptr<conn>> _store;
            std::function<std::unique_ptr<handler> ()> _factory;
        };
    }
}