/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_runloop.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_event.hpp>

namespace chen
{
    namespace tcp
    {
        class client : public basic
        {
        public:
            enum class State : std::uint8_t {Connecting = 1, Connected, Disconnect};
            enum class Event : std::uint8_t {Connected = 1, Disconnect, Read, Write};

        public:
            client(net::runloop &runloop);
            ~client();

        public:
            /**
             * Connect to first resolved endpoint
             * @attention throw exception if no dns record found or dns error
             */
            void connect(const char *mixed);
            void connect(const std::string &mixed, ip::address::Type type = ip::address::Type::None);
            void connect(const std::string &host, std::uint16_t port, ip::address::Type type = ip::address::Type::None);
            void connect(const std::string &host, const std::string &service, ip::address::Type type = ip::address::Type::None);

            /**
             * Connect to remote endpoint
             */
            void connect(const net::endpoint &ep);

            /**
             * Disconnect and reconnect to remote
             * @attention this method will not trigger the disconnect callback
             */
            void reconnect();

            /**
             * Close the connection and clear the buffer
             * @attention this method will not trigger the disconnect callback
             */
            void disconnect();

        public:
//            /**
//             * Read data from remote host
//             * the read callback will be invoked if successful
//             * @param size the desired read length, actual size will be less or equal than this value
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
//             * @attention the delimiter will be removed in buffer, so no "\r\n", '\n', '\r' in the end
//             */
//            void readLine();
//
//            /**
//             * Read until read a certain amount of data
//             */
//            void readUntil(std::size_t size);
//
//            /**
//             * Read until meet the text
//             */
//            void readUntil(const std::string &text);

            /**
             * Write data to remote host
             * you can safely call this method even if the socket is not connected yet
             * the data will be sent immediately after the connection is established
             */
            void write(const void *data, std::size_t size);

        public:
            /**
             * Check connection
             */
            bool isConnecting() const;
            bool isConnected() const;
            bool isDisconnect() const;

            /**
             * Remote endpoint
             */
            net::endpoint remote() const;

        public:
            /**
             * Attach & Detach event callbacks
             * :-) connected  : void (connected_event event)  -> connect to remote SUCCESS or FAILURE
             * :-) disconnect : void (disconnect_event event) -> established connection is broken
             * :-) read       : void (read_event event)       -> read data from remote
             * :-) write      : void (write_event event)      -> write data to remote host
             */
            void attach(std::function<void (tcp::connected_event event)> callback);
            void attach(std::function<void (tcp::disconnect_event event)> callback);
            void attach(std::function<void (tcp::read_event event)> callback);
            void attach(std::function<void (tcp::write_event event)> callback);

            void detach(Event type);

        protected:
            /**
             * Notify events
             */
            void notify(tcp::connected_event &&event);
            void notify(tcp::disconnect_event &&event);
            void notify(tcp::read_event &&event);
            void notify(tcp::write_event &&event);

            /**
             * Event handler
             */
            void onReadable();
            void onWritable();
            void onEnded();
            void onEvent(net::runloop::Event type);

        protected:
            State _state = State::Disconnect;

            net::endpoint _remote;
            net::runloop &_runloop;

            std::function<void (tcp::connected_event event)>  _cb_connected;
            std::function<void (tcp::disconnect_event event)> _cb_disconnect;
            std::function<void (tcp::read_event event)>       _cb_read;
            std::function<void (tcp::write_event event)>      _cb_write;
        };
    }
}