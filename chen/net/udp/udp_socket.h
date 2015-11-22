/**
 * Created by Jian Chen
 * @since  2015.11.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace chen
{
    namespace udp
    {
        class socket
        {
        public:
            socket();
            virtual ~socket();

        public:
            /**
             * Send packet to remote
             */
            virtual void send(const std::uint8_t *data,
                              std::size_t size,
                              const std::string &addr,
                              std::uint16_t port);

            /**
             * Recv packet from remote
             * @param size buffer size & received length
             * @param addr received remote address
             * @param port received remote port
             */
            virtual void recv(std::uint8_t *data,
                              std::size_t &size,
                              std::string &addr,
                              std::uint16_t &port);

            /**
             * Close socket
             */
            virtual void close();

        protected:
            /**
             * Build socket
             */
            virtual void build();

        private:
            socket(const socket &) = delete;
            socket& operator=(const socket &) = delete;

        protected:
            struct impl;
            std::unique_ptr<impl> _impl;
        };
    }
}