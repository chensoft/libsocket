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
            void connect(const endpoint &ep);
            void connect(const ip::address &addr, std::uint16_t port);

            /**
             * Disconnect and reconnect to last host
             * @caution will not trigger the disconnect callback
             */
            void reconnect();

            /**
             * Close the connection and clear the buffer
             * @caution will not trigger the disconnect callback
             */
            void disconnect();

        public:
            /**
             * Write data to connected host
             * you can safely call this method even if the socket is not connected yet
             * the data will be sent immediately after the socket is connected successfully
             */
            void write(const char *data, std::size_t size);

            /**
             * Read data from connected host
             * data will be sent to user via the read callback
             * @param size the desired received length, actual size will be less or equal than this value
             */
            void read(std::size_t size);

            /**
             * Read all data until eof
             */
            void readAll();

            /**
             * Read a line until meet '\n', "\r\n" or eof
             * @caution the delimiter will be removed, so no '\n', "\r\n" in the end
             */
            void readLine();

            /**
             * Read until received a certain amount of data
             */
            void readUntil(std::size_t size);

            /**
             * Read until meet the text
             */
            void readUntil(const std::string &text);

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