/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_policy.hpp>
#include <socket/tcp/tcp_basic.hpp>
#include <memory>

namespace chen
{
    namespace tcp
    {
        class handler;

        class conn : public basic
        {
        public:
            /**
             * Construct by socket fd and handler object
             * it's the server's responsibility to allocate this object
             * server will add this object to its connection pool
             * so you don't need to release this object manually
             */
            conn(bsd_socket &&s, std::unique_ptr<handler> &&h);
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
             */
            void write(const char *text);
            void write(const std::string &text);
            void write(const void *data, std::size_t size);

        public:
            /**
             * Check connection
             */
            bool isConnected() const;
            bool isDisconnect() const;

        protected:
            friend class server;

            /**
             * Native socket handle
             */
            socket_t native() const;

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
            void onAccepted();
            void onReadable();
            void onWritable();
            void onEnded();

        protected:
            bool _connected = true;

            std::unique_ptr<policy> _policy;
            std::unique_ptr<handler> _handler;

            std::vector<std::uint8_t> _buf_read;
            std::vector<std::uint8_t> _buf_write;
        };
    }
}