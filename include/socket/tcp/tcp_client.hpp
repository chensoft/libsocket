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
            virtual bool connect(const std::string &addr, std::uint16_t port, float timeout = 0);

            /**
             * Reconnect use last remote info
             */
            virtual bool reconnect(float timeout = 0);

            /**
             * Shutdown and close
             */
            virtual void disconnect();

        public:
            /**
             * Recent connected remote address and port
             */
            virtual std::string   recentAddr() const;
            virtual std::uint16_t recentPort() const;

            /**
             * Is connected
             */
            virtual bool isConnected() const;

            /**
             * Is blocking or non-blocking
             */
            virtual bool isBlocking() const;
            virtual bool isNonBlocking() const;

        public:
            /**
             * Close socket
             */
            virtual void close() override;

        protected:
            std::string   _recent_addr;
            std::uint16_t _recent_port = 0;

            bool _connected = false;
        };
    }
}