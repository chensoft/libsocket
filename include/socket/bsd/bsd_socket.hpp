/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_address.hpp>
#include <socket/bsd/bsd_option.hpp>
#include <socket/bsd/bsd_setup.hpp>
#include <vector>

namespace chen
{
    namespace bsd
    {
        class socket
        {
        public:
            socket(socket_t fd);
            socket(int domain, int type, int protocol);

            socket(socket &&o);
            socket& operator=(socket &&o);

            ~socket();

        public:
            /**
             * Connect to remote address
             */
            chen::status connect(const bsd::address &addr);

            /**
             * Bind on specific address
             */
            chen::status bind(const bsd::address &addr);

            /**
             * Listen for request
             * @param backlog maximum queue length for pending connections
             */
            chen::status listen();
            chen::status listen(int backlog);

            /**
             * Accept new request and create a new socket
             */
            socket_t accept();

        public:
            /**
             * Send data to connected host, usually used in tcp
             */
            ssize_t send(const void *data, std::size_t size, int flags = 0);

            /**
             * Send data to specific host, usually used in udp
             */
            ssize_t send(const void *data, std::size_t size, const bsd::address &addr, int flags = 0);

            /**
             * Receive data from connected host, usually used in tcp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags = 0);

            /**
             * Receive data from specific host, usually used in udp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, bsd::address &addr, int flags = 0);

        public:
            /**
             * Stop send or receive, but socket is still connected
             */
            void shutdownBoth();
            void shutdownRead();
            void shutdownWrite();

            /**
             * Close the socket, the socket will disconnect immediately
             */
            void close();

        public:
            /**
             * Local and remote address
             */
            bsd::address local() const;
            bsd::address remote() const;

            /**
             * Non-blocking status
             */
            bool nonblocking() const;
            bool nonblocking(bool enable);

            /**
             * Get/Set socket option
             * Usage:
             * >> auto opt = socket.option();
             * >> opt.reuseaddr(true);           // set option
             * >> bool reuse = opt.reuseaddr();  // get option
             */
            bsd::option option();

            /**
             * Native socket handle
             */
            socket_t native() const;

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        private:
            socket_t _fd = socket_t();  // socket descriptor
        };
    }
}