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
            enum class Event : std::uint8_t {Disconnect = 1, Read, Write};

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
             * @attention this method will not trigger the disconnect callback
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
             * @attention the delimiter will be removed in buffer, so no "\r\n", '\n', '\r' in the end
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
            bool isConnected() const;

        public:
            /**
             * Attach & Detach event callbacks
             * :-) disconnect : void (client &c, disconnect_event &e) -> established connection is broken
             * :-) read       : void (client &c, read_event &e)       -> read data from remote
             * :-) write      : void (client &c, write_event &e)      -> write data to remote host
             */
            void attach(std::function<void (chen::tcp::conn &c, chen::tcp::disconnect_event &e)> callback);
            void attach(std::function<void (chen::tcp::conn &c, chen::tcp::read_event &e)> callback);
            void attach(std::function<void (chen::tcp::conn &c, chen::tcp::write_event &e)> callback);

            void detach(Event type);

        protected:
            /**
             * Notify events
             */
            void notify(tcp::disconnect_event &&ev);
            void notify(tcp::read_event &&ev);
            void notify(tcp::write_event &&ev);

            /**
             * Event callbacks
             */
            virtual void onRead(std::vector<std::uint8_t> data, std::error_code error) override;
            virtual void onWrite(std::size_t size, std::error_code error) override;
            virtual void onEnd(std::error_code error) override;

        public:
            tcp::server *_manager = nullptr;

            std::function<void (chen::tcp::conn &c, chen::tcp::disconnect_event &e)> _cb_disconnect;
            std::function<void (chen::tcp::conn &c, chen::tcp::read_event &e)>       _cb_read;
            std::function<void (chen::tcp::conn &c, chen::tcp::write_event &e)>      _cb_write;
        };
    }
}