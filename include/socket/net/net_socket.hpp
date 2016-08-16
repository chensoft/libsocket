/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>
#include <system_error>

namespace chen
{
    namespace net
    {
        /**
         * This class is a wrapper for BSD socket related functions
         * you should not use this class directly, use class like tcp::server instead
         * @caution you can retrieve last error info use the error() method
         */
        class socket
        {
        public:
            /**
             * Common socket type
             */
            enum class Family {IPv4, IPv6, Unix};
            enum class Protocol {TCP, UDP, RAW};

        public:
            /**
             * Construct socket
             * todo add reset method
             */
            socket(Family family, Protocol protocol);
            ~socket();

        public:
            /**
             * Connect to remote host
             */
            void connect(const endpoint &ep);

            /**
             * Bind on specific endpoint
             */
            void bind(const endpoint &ep);

            /**
             * Listen for request
             * @param backlog maximum queue length for pending connections
             */
            void listen(int backlog);

            /**
             * Accept new request
             */
            void accept(endpoint &ep);

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
             * Stop send or receive, but socket is still connected
             */
            void shutdown();

            /**
             * Close the socket, the socket will disconnect immediately
             */
            void close();

        public:
            /**
             * Last error info, compare it with std::errc enum class
             * e.g: socket.error() == std::errc::operation_would_block
             */
            std::error_code error() const;

            /**
             * Socket info
             */
            Family family() const;
            Protocol protocol() const;

            /**
             * Local and remote endpoint
             */
            endpoint local() const;
            endpoint remote() const;

            /**
             * Query connection status
             */
            bool isConnected() const;
            bool isConnecting() const;

        public:
            /**
             * Resolve host address
             */
            static address resolve(const std::string &host);

        private:
            socket(const socket&) = delete;
            socket& operator=(const socket&) = delete;

        private:
            int _fd;  // socket file descriptor(aka socket handle)

            Family   _family;
            Protocol _protocol;
        };
    }
}