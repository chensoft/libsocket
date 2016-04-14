/**
 * Created by Jian Chen
 * @since  2016.04.10
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "tcp_socket.hpp"
#include "tcp_conn.hpp"
#include <functional>

namespace chen
{
    namespace tcp
    {
        class server : public socket
        {
        public:
            /**
             * Receive Callback
             */
            typedef std::function<void (std::unique_ptr<chen::tcp::conn> conn)> callback_type;

        public:
            /**
             * Start the server
             */
            virtual void start();
            virtual void start(const std::string &addr, std::uint16_t port);

            /**
             * Stop the server
             */
            virtual void stop();

        public:
            /**
             * Set observer for server
             */
            virtual void attach(callback_type callback);

            /**
             * Set observer to empty
             */
            virtual void detach();

            /**
             * Notify the observer
             */
            virtual void notify(std::unique_ptr<chen::tcp::conn> conn);

        public:
            /**
             * Bind socket to specific port
             * use `start` method is more convenient
             */
            virtual void bind(const std::string &addr, std::uint16_t port);

            /**
             * Listen on the socket
             * use `start` method is more convenient
             */
            virtual void listen();

            /**
             * Accept new connection
             * use `start` method is more convenient
             */
            virtual std::unique_ptr<chen::tcp::conn> accept();

        protected:
            callback_type _callback;
        };
    }
}