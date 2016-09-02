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
            bool connect(const endpoint &ep);
            bool connect(const ip::address &addr, std::uint16_t port);

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
             * Send data to connected host
             * todo make flags to enum class
             * todo add deadline
             */
            using basic::send;
            ssize_t send(const std::vector<std::uint8_t> &data, int flags = 0);

            /**
             * Receive data from connected host
             */
            using basic::recv;
            std::vector<std::uint8_t> recv(std::size_t size, int flags = 0);

        public:
            /**
             * Check connection
             */
            bool isConnected() const;
            bool isConnecting() const;

            /**
             * Stored host and port
             */
            std::string host() const;
            std::uint16_t port() const;

        private:
            std::string _host;
            std::uint16_t _port = 0;
        };
    }
}