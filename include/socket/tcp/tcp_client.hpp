/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_basic.hpp>

namespace chen
{
    namespace tcp
    {
        class client : public basic
        {
        public:
            /**
             * Connect to remote host
             */
            bool connect(const net::endpoint &ep);
            bool connect(const net::address &addr, std::uint16_t port);

            /**
             * Reconnect to last host
             */
            bool reconnect();

            /**
             * Close the connection
             */
            bool disconnect();

        public:
            /**
             * Send data to connected host, usually used in tcp
             * todo make flags to enum class
             * todo add deadline
             */
            ssize_t send(const void *data, std::size_t size, int flags);
            ssize_t send(const std::vector<std::uint8_t> &data, int flags);

            /**
             * Receive data from connected host, usually used in tcp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags);
            std::vector<std::uint8_t> recv(std::size_t size, int flags);

        public:
            /**
             * Check connection
             */
            bool isConnected() const;
            bool isConnecting() const;
        };
    }
}