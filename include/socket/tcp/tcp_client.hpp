/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_event.hpp>

namespace chen
{
    namespace tcp
    {
        class client : public basic
        {
        public:
            enum class State {Disconnect = 0, Connecting, Connected};

            typedef std::function<void (chen::tcp::client &c, chen::tcp::event::basic *ev)> callback_type;

        public:
            /**
             * Connect to remote host
             * todo how to deal with domain resolve error
             */
            void connect(const endpoint &ep);
            void connect(const ip::address &addr, std::uint16_t port);

            /**
             * Disconnect and reconnect to last host
             * @notice this method will not trigger the disconnect callback
             */
            void reconnect();

            /**
             * Close the connection and clear the buffer
             * @notice this method will not trigger the disconnect callback
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
             * Read a line until meet "\r\n", '\n', '\r' or eof
             * @notice the delimiter will be removed, so no "\r\n", '\n', '\r' in the end
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
             * Set callback for client
             * many events will be triggered:
             * :-) connecting: when user issue the connect command
             * :-) connected: when connected to the host success or failure
             * :-) disconnect: connection is broken
             * :-) read: received some data
             * :-) write: write data to socket successfully
             * @notice if host has multiple addresses, client will try to connect each address until success
             * every time client will notify the status via connecting & connected callback
             */
            void attach(callback_type cb);

            /**
             * Remove callback
             */
            void detach();

        public:
            /**
             * Check connection
             */
            State state() const;

            bool isDisconnect() const;
            bool isConnecting() const;
            bool isConnected() const;

            /**
             * Stored host and port
             */
            std::string host() const;
            std::uint16_t port() const;

        protected:
            /**
             * Notify
             * todo shorten the method name
             */
            void notifyConnecting(endpoint ep);
            void notifyConnected(endpoint ep, chen::status err);
            void notifyDisconnect(chen::status err);
            void notifyRead(std::vector<std::uint8_t> data);
            void notifyWrite(std::size_t size);

            /**
             * Overwrite
             */
            virtual void onEvent(chen::notifier &n, chen::notifier::Event ev) override;

        private:
            State _state = State::Disconnect;

            std::string   _host;
            std::uint16_t _port = 0;

            callback_type _cb;
        };
    }
}