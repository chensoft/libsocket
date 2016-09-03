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
            enum class State {None = 0, Connected, Connecting};

        public:
            /**
             * Connect to remote host
             */
            std::error_code connect(const endpoint &ep);
            std::error_code connect(const ip::address &addr, std::uint16_t port);

            /**
             * Reconnect to last host
             */
            std::error_code reconnect();

            /**
             * Close the connection
             */
            std::error_code disconnect();

        public:
            /**
             * Send data to connected host
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
            State state() const;
            bool isConnected() const;
            bool isConnecting() const;

            /**
             * Stored host and port
             */
            std::string host() const;
            std::uint16_t port() const;

        private:
            State _state = State::None;

            std::string   _host;
            std::uint16_t _port = 0;
        };
    }
}