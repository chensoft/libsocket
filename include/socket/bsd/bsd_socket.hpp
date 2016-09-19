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
        class socket final
        {
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

            socket(socket &&o) noexcept;
            socket& operator=(socket &&o) noexcept;

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
            ssize_t recv(std::vector<std::uint8_t> &out, int flags = 0) noexcept;

            /**
             * Receive data from specific host, usually used in udp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, bsd::endpoint &ep, int flags = 0) noexcept;

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

        public:
            /**
             * Local and remote endpoint
             */
            void local(bsd::endpoint &ep) const noexcept;
            void remote(bsd::endpoint &ep) const noexcept;

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
             */
            bool valid() const noexcept;
            operator bool() const noexcept;

            /**
             * Native socket handle
             */
            socket_t native() const noexcept;

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        private:
            socket_t _fd = socket_t(-1);  // socket descriptor
        };
    }
}