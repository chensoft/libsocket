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
            enum class Event {Connecting = 1, Connected, Disconnect, Send, Recv};

        public:
            // todo how to reset socket if disconnect
            client(ip::address::Type family, net::proactor &proactor);

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
             * Send data to remote host
             * you can safely call this method even if the socket is not connected yet
             * the data will be sent immediately after the connection is successful
             */
            void send(const char *data, std::size_t size);

            /**
             * Receive data from remote host
             * the recv callback will be invoked if successful
             * @param size the desired received length, actual size will be less or equal than this value
             */
            void recv(std::size_t size);

            /**
             * Receive all data until eof
             */
            void recvAll();

            /**
             * Receive a line until meet "\r\n", '\n', '\r' or eof
             * @notice the delimiter will be removed in buffer, so no "\r\n", '\n', '\r' in the end
             */
            void recvLine();

            /**
             * Receive until received a certain amount of data
             */
            void recvUntil(std::size_t size);

            /**
             * Receive until meet the text
             */
            void recvUntil(const std::string &text);

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
             * :-) connecting : void (client &c, connecting_event &e) -> when user issue the connect command
             * :-) connected  : void (client &c, connected_event &e)  -> connect to remote success or failure
             * :-) disconnect : void (client &c, disconnect_event &e) -> established connection is broken
             * :-) send       : void (client &c, send_event &e)       -> send data to remote host
             * :-) recv       : void (client &c, recv_event &e)       -> receive data from remote
             */
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::connecting_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::connected_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::disconnect_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::send_event &e)> callback);
            void attach(std::function<void (chen::tcp::client &c, chen::tcp::recv_event &e)> callback);

            void detach(Event type);

        protected:
            /**
             * Notify events
             */
            void notify(tcp::connecting_event &&ev);
            void notify(tcp::connected_event &&ev);
            void notify(tcp::disconnect_event &&ev);
            void notify(tcp::send_event &&ev);
            void notify(tcp::recv_event &&ev);

            /**
             * Event callbacks
             */
            virtual void onEventSend(std::size_t size, std::error_code error) override;
            virtual void onEventRecv(std::vector<std::uint8_t> data, std::error_code error) override;
            virtual void onEventEOF() override;

        public:
            /**
             * Handy methods for creating socket
             */
            static client v4(net::proactor &proactor);
            static client v6(net::proactor &proactor);

        private:
            enum class State {Connecting = 1, Connected, Disconnect};

            State _state = State::Disconnect;

            std::string   _host;
            std::uint16_t _port = 0;

            std::function<void (chen::tcp::client &c, chen::tcp::connecting_event &e)> _cb_connecting;
            std::function<void (chen::tcp::client &c, chen::tcp::connected_event &e)>  _cb_connected;
            std::function<void (chen::tcp::client &c, chen::tcp::disconnect_event &e)> _cb_disconnect;
            std::function<void (chen::tcp::client &c, chen::tcp::send_event &e)>       _cb_send;
            std::function<void (chen::tcp::client &c, chen::tcp::recv_event &e)>       _cb_recv;

            net::proactor &_proactor;
        };
    }
}