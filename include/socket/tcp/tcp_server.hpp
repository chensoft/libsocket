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
             * Start by port, use IPv6 by default
             * @attention modern os support both v4 & v6 traffic if you bind socket by AF_INET6
             */
            void start(std::uint16_t port, ip::address::Type type = ip::address::Type::IPv6);

            /**
             * Start by first resolved endpoint
             * @attention throw exception if no dns record found or dns error
             */
            void start(const char *mixed);
            void start(const std::string &mixed, ip::address::Type type = ip::address::Type::None);
            void start(const std::string &host, std::uint16_t port, ip::address::Type type = ip::address::Type::None);
            void start(const std::string &host, const std::string &service, ip::address::Type type = ip::address::Type::None);

            /**
             * Start by endpoint
             * @attention server will be nonblocking and SO_REUSEADDR is true
             */
            void start(const net::endpoint &ep);

            /**
             * Stop the server
             */
            void stop();

//        public:
//            /**
//             * Attach accept callback
//             */
//            void attach(std::function<void (chen::tcp::server &s, std::shared_ptr<chen::tcp::conn> conn)> callback);
//
//            /**
//             * Detach the callback
//             */
//            void detach();
//
//        protected:
//            /**
//             * Emit the callback
//             */
//            void notify(std::shared_ptr<chen::tcp::conn> &&conn);
//
//            /**
//             * Event callbacks
//             */
//            void onAccept(bsd::socket s, net::endpoint ep) override;
//            void onRead(std::vector<std::uint8_t> data, net::endpoint ep, std::error_code error) override;
//            void onEnd() override;
//
//        public:
//            /**
//             * Bind & Listen
//             * @attention usually you don't need to call these methods, use start() directly
//             */
//            std::error_code bind(const net::endpoint &ep);
//
//            std::error_code listen();
//            std::error_code listen(int backlog);
//
        protected:
//            std::set<std::shared_ptr<chen::tcp::conn>> _connections;
//            std::function<void (chen::tcp::server &s, std::shared_ptr<chen::tcp::conn> conn)> _callback;
        };
    }
}