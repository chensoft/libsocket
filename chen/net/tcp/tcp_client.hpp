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
             */
            virtual void connect(const std::string &addr, std::uint16_t port, float timeout = 0);

            /**
             * Reconnect use last remote info
             */
            virtual void reconnect(float timeout = 0);

            /**
             * Shutdown and close
             */
            virtual void disconnect();

        public:
            /**
             * Is connected
             */
            virtual bool connected() const;

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