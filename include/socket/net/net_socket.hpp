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
            enum class Family {IPv4, IPv6, Unix};
            enum class Protocol {TCP, UDP, RAW};
            enum class Shutdown {Read, Write, Both};

        public:
            /**
             * Construct socket
             */
            socket(Family family, Protocol protocol);
            ~socket();

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
             * Accept new request
             */
            endpoint accept() noexcept;

        public:
            /**
             * Send data to connected host, usually used in tcp
             */
            void send(const std::vector<std::uint8_t> &data, int flags);
            void send(std::vector<std::uint8_t> &&data, int flags);

            /**
             * Send data to specific host, usually used in udp
             */
            void send(const std::vector<std::uint8_t> &data, int flags, const endpoint &ep);
            void send(std::vector<std::uint8_t> &&data, int flags, const endpoint &ep);

            /**
             * Receive data from connected host, usually used in tcp
             */
            std::vector<std::uint8_t> recv(std::size_t size, int flags);

            /**
             * Receive data from specific host, usually used in udp
             */
            std::vector<std::uint8_t> recv(std::size_t size, int flags, endpoint &ep);

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
            static address resolve(const std::string &host);

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