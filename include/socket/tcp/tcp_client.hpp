/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_event.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <unordered_map>

namespace chen
{
    namespace tcp
    {
        class client : public basic
        {
        public:
            client(socket_t fd);
            client(ip::address::Type family);

        public:
            /**
             * Connect to remote host
             * todo how to deal with domain resolve error
             */
            void connect(const net::endpoint &ep);
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
             * Check connection
             */
            bool isDisconnect() const;
            bool isConnecting() const;
            bool isConnected() const;

            /**
             * Stored host and port
             */
            std::string host() const;
            std::uint16_t port() const;

        public:
            /**
             * Attach & Detach event callbacks
             * :-) connecting : void (client&, connecting_event&) -> when user issue the connect command
             * :-) connected  : void (client&, connected_event&)  -> connect to remote success or failure
             * :-) disconnect : void (client&, disconnect_event&) -> established connection is broken
             * :-) send       : void (client&, send_event&)       -> send data to remote host
             * :-) recv       : void (client&, recv_event&)       -> receive data from remote
             */
            void attach(tcp::event::Type type, std::function<void (chen::tcp::client &c, chen::tcp::event &e)> callback);
            void detach(tcp::event::Type type);

        protected:
            /**
             * Event callbacks
             */
            virtual void onEventSend(std::size_t size, std::error_code error);
            virtual void onEventRecv(std::vector<std::uint8_t> data, std::error_code error);
            virtual void onEventEOF();

        public:
            /**
             * Handy methods for creating socket
             */
            static client v4();
            static client v6();

        private:
            enum class State {Connecting = 1, Connected, Disconnect};
            State _state = State::Disconnect;

            std::string   _host;
            std::uint16_t _port = 0;

            std::unordered_map<int, std::function<void (chen::tcp::client &c, chen::tcp::event &e)>> _event;

//        public:
//            /**
//             * Write data to connected host
//             * you can safely call this method even if the socket is not connected yet
//             * the data will be sent immediately after the socket is connected successfully
//             */
//            void write(const char *data, std::size_t size);
//
//            /**
//             * Read data from connected host
//             * data will be sent to user via the read callback
//             * @param size the desired received length, actual size will be less or equal than this value
//             */
//            void read(std::size_t size);
//
//            /**
//             * Read all data until eof
//             */
//            void readAll();
//
//            /**
//             * Read a line until meet "\r\n", '\n', '\r' or eof
//             * @notice the delimiter will be removed, so no "\r\n", '\n', '\r' in the end
//             */
//            void readLine();
//
//            /**
//             * Read until received a certain amount of data
//             */
//            void readUntil(std::size_t size);
//
//            /**
//             * Read until meet the text
//             */
//            void readUntil(const std::string &text);
//
//        protected:
//            /**
//             * Notify
//             * todo shorten the method name
//             */
//            void notifyConnecting(endpoint ep);
//            void notifyConnected(endpoint ep, std::error_code err);
//            void notifyDisconnect(std::error_code err);
//            void notifyRead(std::vector<std::uint8_t> data);
//            void notifyWrite(std::size_t size);
        };
    }
}