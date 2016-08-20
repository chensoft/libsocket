/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>
#include <system_error>
#include <memory>

namespace chen
{
    namespace net
    {
        /**
         * This class is a wrapper for BSD socket
         * you should not use this class directly
         * use classes like tcp::server or udp::server instead
         */
        class socket
        {
        public:
            enum class Family {IPv4 = 1, IPv6, Unix};
            enum class Protocol {TCP = 1, UDP, RAW};
            enum class Shutdown {Read = 1, Write, Both};

        public:
            /**
             * Construct socket
             */
            socket(Family family, Protocol protocol);
            ~socket();

            socket(socket&&) = default;
            socket& operator=(socket&&) = default;

        public:
            /**
             * Connect to remote host
             */
            bool connect(const endpoint &ep) noexcept;
            bool connect(const address &addr, std::uint16_t port) noexcept;

            /**
             * Bind on specific endpoint
             */
            bool bind(const endpoint &ep) noexcept;
            bool bind(const address &addr, std::uint16_t port) noexcept;

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
             * todo make flags to enum class
             */
            ssize_t send(const void *data, std::size_t size, int flags) noexcept;
            ssize_t send(const std::vector<std::uint8_t> &data, int flags) noexcept;

            /**
             * Send data to specific host, usually used in udp
             */
            ssize_t send(const void *data, std::size_t size, int flags, const endpoint &ep) noexcept;
            ssize_t send(const std::vector<std::uint8_t> &data, int flags, const endpoint &ep) noexcept;

            /**
             * Receive data from connected host, usually used in tcp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags) noexcept;
            std::vector<std::uint8_t> recv(std::size_t size, int flags) noexcept;

            /**
             * Receive data from specific host, usually used in udp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags, endpoint &ep) noexcept;
            std::vector<std::uint8_t> recv(std::size_t size, int flags, endpoint &ep) noexcept;

        public:
            /**
             * todo set opt for socket
             */

        public:
            /**
             * Reset this socket, close old socket and create new
             */
            void reset();
            void reset(Family family, Protocol protocol);

            /**
             * Close the socket, the socket will disconnect immediately
             * todo block in TIME_WAIT?
             */
            bool close() noexcept;

            /**
             * Stop send or receive, but socket is still connected
             */
            bool shutdown(Shutdown flag) noexcept;

        public:
            /**
             * Last error info, compare it with std::errc enum class
             * e.g: socket.error() == std::errc::operation_would_block
             */
            std::error_code error() const noexcept;

            /**
             * Socket info
             */
            Family family() const noexcept;
            Protocol protocol() const noexcept;

            /**
             * Local and remote endpoint
             */
            endpoint local() const noexcept;
            endpoint remote() const noexcept;

        public:
            /**
             * Resolve host address
             */
            static std::vector<address> resolve(const std::string &host) noexcept;

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        private:
            Family   _family;
            Protocol _protocol;

            struct impl;
            std::unique_ptr<impl> _impl;
        };
    }
}