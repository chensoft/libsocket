/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>

namespace chen
{
    namespace net
    {
        /**
         * This class is a wrapper for BSD socket, you should not use this class directly
         * use specific classes like tcp::server, tcp::client, udp::server and udp::client instead
         */
        class socket
        {
        public:
            enum class Shutdown {Read = 1, Write, Both};

        public:
            /**
             * Construct socket
             */
            socket(std::nullptr_t t = nullptr);

            socket(socket_t fd);
            socket(int family, int type, int protocol = 0);
            socket(address addr, int type, int protocol = 0);

            socket(socket &&o);
            socket& operator=(socket &&o);

            ~socket();

        public:
            /**
             * Reset socket
             */
            void reset(socket_t fd);
            void reset(int family, int type, int protocol = 0);
            void reset(address addr, int type, int protocol = 0);

        public:
            /**
             * Connect to remote host
             */
            bool connect(const endpoint &ep) noexcept;

            /**
             * Bind on specific endpoint
             */
            bool bind(const endpoint &ep) noexcept;

            /**
             * Listen for request
             * @param backlog maximum queue length for pending connections
             */
            bool listen() noexcept;
            bool listen(int backlog) noexcept;

            /**
             * Accept new request and create a new socket
             */
            socket accept();

        public:
            /**
             * Send data to connected host, usually used in tcp
             */
            ssize_t send(const void *data, std::size_t size, int flags = 0) noexcept;
            ssize_t send(const std::vector<std::uint8_t> &data, int flags = 0) noexcept;

            /**
             * Send data to specific host, usually used in udp
             */
            ssize_t send(const void *data, std::size_t size, const endpoint &ep, int flags = 0) noexcept;
            ssize_t send(const std::vector<std::uint8_t> &data, const endpoint &ep, int flags = 0) noexcept;

            /**
             * Receive data from connected host, usually used in tcp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags = 0) noexcept;
            std::vector<std::uint8_t> recv(std::size_t size, int flags = 0) noexcept;

            /**
             * Receive data from specific host, usually used in udp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, endpoint &ep, int flags = 0) noexcept;
            std::vector<std::uint8_t> recv(std::size_t size, endpoint &ep, int flags = 0) noexcept;

        public:
            /**
             * Close the socket, the socket will disconnect immediately
             */
            bool close() noexcept;

            /**
             * Stop send or receive, but socket is still connected
             */
            bool shutdown(Shutdown flag) noexcept;

        public:
            /**
             * Last errno code, compare it with std::errc enum class
             * e.g: socket.error() == std::errc::operation_would_block
             */
            std::error_code error() const noexcept;

            /**
             * Local and remote endpoint
             */
            endpoint local() const noexcept;
            endpoint remote() const noexcept;

            /**
             * Native socket handle
             */
            socket_t native() const noexcept;

            /**
             * Non-blocking status
             */
            bool nonblocking() const noexcept;
            bool nonblocking(bool enable) noexcept;

        public:
            /**
             * Check socket is empty
             */
            bool empty() const noexcept;
            operator bool() const noexcept;

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        private:
            socket_t _fd = socket_t();  // socket descriptor
        };
    }
}