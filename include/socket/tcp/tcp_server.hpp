/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_proactor.hpp>
#include <socket/ip/ip_address.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_conn.hpp>
#include <functional>
#include <memory>
#include <set>

namespace chen
{
    namespace tcp
    {
        class server : public basic
        {
        public:
            server(ip::address::Type family);

        public:
            /**
             * Start the server
             * todo return type change to std::error_code, transfer exit error code to user
             */
            void start(const net::endpoint &ep);

            /**
             * Stop the server
             */
            void stop();

        public:
            /**
             * Attach accept callback
             */
            void attach(std::function<void (chen::tcp::server &s, std::shared_ptr<chen::tcp::conn> conn)> callback);

            /**
             * Detach the callback
             */
            void detach();

        protected:
            /**
             * Emit the callback
             */
            void notify(std::shared_ptr<chen::tcp::conn> &&conn);

            /**
             * Event callbacks(only onEventRead is useful)
             */
            virtual void onRead(std::vector<std::uint8_t> data, net::endpoint ep, std::error_code error) override;
            virtual void onWrite(std::size_t size, net::endpoint ep, std::error_code error) override;
            virtual void onEnd() override;

        public:
            /**
             * Bind & Listen
             * @attention usually you don't need to call these methods, use start() directly
             */
            std::error_code bind(const net::endpoint &ep);

            std::error_code listen();
            std::error_code listen(int backlog);

        protected:
            net::proactor _proactor;
            std::set<std::shared_ptr<chen::tcp::conn>> _connections;
            std::function<void (chen::tcp::server &s, std::shared_ptr<chen::tcp::conn> conn)> _callback;
        };
    }
}