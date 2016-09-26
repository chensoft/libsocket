/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_proactor.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <socket/tcp/tcp_event.hpp>
#include <functional>

namespace chen
{
    namespace tcp
    {
        class server;

        class conn : public basic
        {
        public:
            enum class Event : std::uint8_t {Disconnect = 1, Send, Recv};

        public:
            /**
             * Construct by socket fd and server pointer
             * It's the server's responsibility to allocate the conn object
             * server will add this object to its connection pool so you don't need to release this object manually
             */
            conn(socket_t fd, tcp::server *s);

            ~conn();

        public:
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
            bool isConnected() const;

        public:
            /**
             * Attach & Detach event callbacks
             * :-) disconnect : void (client &c, disconnect_event &e) -> established connection is broken
             * :-) send       : void (client &c, send_event &e)       -> send data to remote host
             * :-) recv       : void (client &c, recv_event &e)       -> receive data from remote
             */
            void attach(std::function<void (chen::tcp::conn &c, chen::tcp::disconnect_event &e)> callback);
            void attach(std::function<void (chen::tcp::conn &c, chen::tcp::send_event &e)> callback);
            void attach(std::function<void (chen::tcp::conn &c, chen::tcp::recv_event &e)> callback);

            void detach(Event type);

        protected:
            /**
             * Notify events
             */
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
            tcp::server *_manager = nullptr;

            std::function<void (chen::tcp::conn &c, chen::tcp::disconnect_event &e)> _cb_disconnect;
            std::function<void (chen::tcp::conn &c, chen::tcp::send_event &e)>       _cb_send;
            std::function<void (chen::tcp::conn &c, chen::tcp::recv_event &e)>       _cb_recv;
        };
    }
}