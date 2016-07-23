/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "tcp_socket.hpp"

namespace chen
{
    namespace tcp
    {
        class client : public socket
        {
        public:
            client();

        public:
            /**
             * Connect to remote server
             * @result true if connected, false if timeout, exception if occur error
             */
            bool connect(const std::string &addr, std::uint16_t port, float timeout = 0);

            /**
             * Reconnect use last remote info
             */
            bool reconnect(float timeout = 0);

            /**
             * Shutdown and close
             */
            void disconnect();

        public:
            /**
             * Recent connected remote address and port
             */
            std::string   recentAddr() const;
            std::uint16_t recentPort() const;

            /**
             * Is connected
             */
            bool isConnected() const;

            /**
             * Is blocking or non-blocking
             */
            bool isBlocking() const;
            bool isNonBlocking() const;

        public:
            /**
             * Close socket
             */
            void close();

        protected:
            std::string   _recent_addr;
            std::uint16_t _recent_port = 0;

            bool _connected = false;
        };
    }
}