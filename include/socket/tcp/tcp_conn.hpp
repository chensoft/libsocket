/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_basic.hpp>
#include <memory>

namespace chen
{
    namespace tcp
    {
        class handler;

        class conn : public basic
        {
        public:
            /**
             * Construct by socket fd and handler object
             * it's the server's responsibility to allocate this object
             * server will add this object to its connection pool
             * so you don't need to release this object manually
             */
            conn(bsd::socket &&s, std::unique_ptr<handler> &&h);
            ~conn();

        public:
            /**
             * Close the connection and clear the buffer
             * @attention this method will not trigger the disconnect callback
             */
            void disconnect();

        public:
            /**
             * Check connection
             */
            bool isConnected() const;
            bool isDisconnect() const;

        protected:
            friend class server;

            /**
             * Event handler
             */
            void onAccepted();
            void onReadable();
            void onWritable();
            void onEnded();

        protected:
            std::unique_ptr<handler> _handler;
        };
    }
}