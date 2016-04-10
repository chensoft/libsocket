/**
 * Created by Jian Chen
 * @since  2016.04.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
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
            virtual void connect(const std::string &host, std::uint16_t port, float timeout = 0);

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
             * Remote server info
             */
            virtual std::string   remoteHost() const;
            virtual std::string   remoteAddr() const;
            virtual std::uint16_t remotePort() const;

            /**
             * Local client info
             */
            virtual std::string   localAddr() const;
            virtual std::uint16_t localPort() const;

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
            std::string   _remote_host;
            std::string   _remote_addr;
            std::uint16_t _remote_port = 0;

            std::string   _local_addr;
            std::uint16_t _local_port = 0;

            bool _connected = false;
        };
    }
}