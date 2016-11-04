/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/tcp/tcp_policy.hpp>
#include <socket/net/tcp/tcp_basic.hpp>
#include <socket/net/tcp/tcp_event.hpp>
#include <memory>

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
            // todo add param to accept init option
            explicit client(runloop &runloop);
            ~client();

        public:
            /**
             * Connect to first resolved endpoint
             * @attention throw exception if no dns record found or dns error
             */
            void connect(const char *mixed);
            void connect(const std::string &mixed, ip_address::Type type = ip_address::Type::None);
            void connect(const std::string &host, std::uint16_t port, ip_address::Type type = ip_address::Type::None);
            void connect(const std::string &host, const std::string &service, ip_address::Type type = ip_address::Type::None);

            /**
             * Connect to remote endpoint
             */
            void connect(const inet_endpoint &ep);

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
            /**
             * Read data from remote host
             * the read callback will be invoked if successful
             * @param size the desired read length, actual size will be less or equal than this value
             */
            void readSome();
            void readSome(std::size_t size);

            /**
             * Read all data until end
             * every chunk of data will be passed via read callback
             */
            void readAll();

            /**
             * Read a line of data until meet "\r\n", '\n' or eof
             * @attention the delimiter will be removed in buffer, so no "\r\n", '\n' in the end
             */
            void readLine();

            /**
             * Read the exact amount of data
             */
            void readExact(std::size_t size);

            /**
             * Read until meet the text
             * @attention the text will be removed in buffer when returned
             */
            void readUntil(const std::string &text);

            /**
             * Write data to remote host
             * you can safely call this method even if the socket is not connected yet
             * the data will be sent immediately after the connection is established
             */
            void write(const char *text);
            void write(const std::string &text);
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
            inet_endpoint remote() const;

        public:
            /**
             * Attach & Detach event callbacks
             * :-) connected  : void (connected_event event)  -> connect to remote SUCCESS or FAILURE
             * :-) disconnect : void (disconnect_event event) -> established connection is broken
             * :-) read       : void (read_event event)       -> read data from remote
             * :-) write      : void (write_event event)      -> write data to remote host
             */
            void attach(std::function<void (connected_event event)> callback);
            void attach(std::function<void (disconnect_event event)> callback);
            void attach(std::function<void (read_event event)> callback);
            void attach(std::function<void (write_event event)> callback);

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
             * Receive data
             */
            void receive();
            void receive(read_some_policy &policy);
            void receive(read_all_policy &policy);
            void receive(read_line_policy &policy);
            void receive(read_exact_policy &policy);
            void receive(read_until_policy &policy);

            /**
             * Event handler
             */
            void onReadable();
            void onWritable();
            void onEnded();
            void onEvent(runloop::Event type);

        protected:
            State _state = State::Disconnect;

            inet_endpoint _remote;
            runloop &_runloop;

            std::unique_ptr<policy> _policy;

            std::vector<std::uint8_t> _buf_read;
            std::vector<std::uint8_t> _buf_write;

            std::function<void (connected_event event)>  _cb_connected;
            std::function<void (disconnect_event event)> _cb_disconnect;
            std::function<void (read_event event)>       _cb_read;
            std::function<void (write_event event)>      _cb_write;
        };
    }
}