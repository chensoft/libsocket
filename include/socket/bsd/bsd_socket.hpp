/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/bsd_endpoint.hpp>
#include <socket/bsd/bsd_option.hpp>
#include <socket/bsd/bsd_setup.hpp>
#include <vector>

namespace chen
{
    namespace bsd
    {
        /**
         * bsd socket wrapper, usually you don't need to use it directly
         * use tcp::client, tcp::server, udp::client, udp::server instead
         */
        class socket
        {
        public:
            static const socket_t invalid_handle;  // -1 on Unix, INVALID_HANDLE on Windows

        public:
            /**
             * Construct by socket handle directly
             */
            socket(socket_t fd) noexcept;

            /**
             * Construct by socket type
             * @param domain AF_INET, AF_INET6, AF_UNIX...
             * @param type SOCK_STREAM, SOCK_DGRAM, SOCK_RAW...
             * @param protocol IPPROTO_TCP, IPPROTO_UDP or use zero directly
             */
            socket(int domain, int type, int protocol);

            ~socket() noexcept;

        public:
            /**
             * Connect to remote endpoint
             */
            std::error_code connect(const bsd::endpoint &ep) noexcept;

            /**
             * Bind on specific endpoint
             */
            std::error_code bind(const bsd::endpoint &ep) noexcept;

            /**
             * Listen for request
             * @param backlog maximum queue length for pending connections
             */
            std::error_code listen() noexcept;
            std::error_code listen(int backlog) noexcept;

            /**
             * Accept new request and create a new socket
             * @notice check to see if the result is valid before use it
             */
            std::error_code accept(socket_t &fd) noexcept;
            std::error_code accept(socket_t &fd, bsd::endpoint &ep) noexcept;

        public:
            /**
             * Send data to connected host, usually used in tcp
             */
            ssize_t send(const void *data, std::size_t size, int flags = 0) noexcept;

            /**
             * Send data to specific host, usually used in udp
             */
            ssize_t send(const void *data, std::size_t size, const bsd::endpoint &ep, int flags = 0) noexcept;

            /**
             * Receive data from connected host, usually used in tcp
             */
            std::error_code recv(std::vector<std::uint8_t> &out, int flags = 0) noexcept;

            /**
             * Receive data from specific host, usually used in udp
             */
            std::error_code recv(std::vector<std::uint8_t> &out, bsd::endpoint &ep, int flags = 0) noexcept;

        public:
            /**
             * Stop send or receive, but socket is still connected
             */
            void shutdownBoth() noexcept;
            void shutdownRead() noexcept;
            void shutdownWrite() noexcept;

            /**
             * Close the socket, the socket will disconnect immediately
             */
            void close() noexcept;

            /**
             * Reset socket using stored domain, type and protocol
             * @notice if you construct from a socket fd, this method will throw error because it didn't know how to create new socket
             */
            void reset();

        public:
            /**
             * Sock & Peer endpoint
             */
            bsd::endpoint sock() const noexcept;
            bsd::endpoint peer() const noexcept;

            /**
             * Non-blocking status
             */
            bool nonblocking() const noexcept;
            bool nonblocking(bool enable) noexcept;

            /**
             * Get/Set socket option
             * Usage:
             * >> auto opt = socket.option();
             * >> opt.reuseaddr(true);           // set option
             * >> bool reuse = opt.reuseaddr();  // get option
             */
            bsd::option option() noexcept;

            /**
             * Check socket is valid
             * @notice zero is a valid file descriptor
             */
            bool valid() const noexcept;
            operator bool() const noexcept;

            /**
             * Native socket handle
             */
            socket_t native() const noexcept;

        private:
            /**
             * Disable copy & move
             * if you want to store socket in container
             * use std::unique_ptr<bsd::socket> is a choice
             */
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        private:
            int _domain   = 0;  // used for reset socket
            int _type     = 0;
            int _protocol = 0;

            socket_t _fd = socket::invalid_handle;  // socket descriptor
        };
    }
}