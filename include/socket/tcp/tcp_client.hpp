/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_proactor.hpp>
#include <socket/tcp/tcp_event.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <functional>

namespace chen
{
    namespace tcp
    {
        class client : public basic
        {
        public:
            enum class Event : std::uint8_t {Connecting = 1, Connected, Disconnect, Read, Write};

        public:
            // todo how to reset socket if disconnect
            client(ip::address::Type family, net::proactor &proactor);
            ~client();

        public:
            /**
             * Connect to remote host
             * todo how to deal with domain resolve error
             */
            void connect(const net::endpoint &ep);

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
             * Read data from remote host
             * the read callback will be invoked if successful
             * @param size the desired read length, actual size will be less or equal than this value
             */
            void read(std::size_t size);

            /**
             * Read all data until eof
             */
            void readAll();

            /**
             * Read a line until meet "\r\n", '\n', '\r' or eof
             * @notice the delimiter will be removed in buffer, so no "\r\n", '\n', '\r' in the end
             */
            void readLine();

            /**
             * Read until read a certain amount of data
             */
            void readUntil(std::size_t size);

            /**
             * Read until meet the text
             */
            void readUntil(const std::string &text);

            /**
             * Write data to remote host
             * you can safely call this method even if the socket is not connected yet
             * the data will be sent immediately after the connection is successful
             */
            void write(const char *data, std::size_t size);

        public:
            /**
             * Check connection
             */
            bool isDisconnect() const;
            bool isConnecting() const;
            bool isConnected() const;

            /**
             * Remote endpoint
             */
            net::endpoint remote() const;

        public:
            /**
             * Attach & Detach event callbacks
             * :-) connecting : void (client &c, connecting_event &e) -> when user issue the connect command
             * :-) connected  : void (client &c, connected_event &e)  -> connect to remote success or failure
             * :-) disconnect : void (client &c, disconnect_event &e) -> established connection is broken
             * :-) read       : void (client &c, read_event &e)       -> read data from remote
             * :-) write      : void (client &c, write_event &e)      -> write data to remote host
             */
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::connecting_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::connected_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::disconnect_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::read_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::write_event &e)> callback);

            void detach(Event type);

        protected:
            /**
             * Notify events
             */
            void notify(tcp::connecting_event &&ev);
            void notify(tcp::connected_event &&ev);
            void notify(tcp::disconnect_event &&ev);
            void notify(tcp::read_event &&ev);
            void notify(tcp::write_event &&ev);

            /**
             * Event callbacks
             */
            virtual void onRead(std::vector<std::uint8_t> data, std::error_code error) override;
            virtual void onWrite(std::size_t size, std::error_code error) override;
            virtual void onEnd(std::error_code error) override;

        protected:
            enum class State : std::uint8_t {Connecting = 1, Connected, Disconnect};

            State _state = State::Disconnect;

            std::function<void (chen::tcp::client &c, chen::tcp::connecting_event &e)> _cb_connecting;
            std::function<void (chen::tcp::client &c, chen::tcp::connected_event &e)>  _cb_connected;
            std::function<void (chen::tcp::client &c, chen::tcp::disconnect_event &e)> _cb_disconnect;
            std::function<void (chen::tcp::client &c, chen::tcp::write_event &e)>      _cb_write;
            std::function<void (chen::tcp::client &c, chen::tcp::read_event &e)>       _cb_read;

            net::endpoint _remote;
            net::proactor &_proactor;
        };
    }
}