/**
 * Created by Jian Chen
 * @since  2016.08.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_endpoint.hpp>

namespace chen
{
    /**
     * This class is a wrapper for BSD socket, you should not use this class directly
     * use specific classes like tcp::server, tcp::client, udp::server and udp::client instead
     * @caution std::error_code can compare with std::errc enum, e.g: socket.error() == std::errc::permission_denied
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
        socket(ip::address addr, int type, int protocol = 0);

        socket(socket &&o);
        socket& operator=(socket &&o);

        ~socket();

    public:
        /**
         * Reset socket
         */
        void reset(socket_t fd);
        void reset(int family, int type, int protocol = 0);
        void reset(ip::address addr, int type, int protocol = 0);

    public:
        /**
         * Connect to remote host
         */
        std::error_code connect(const endpoint &ep) noexcept;

        /**
         * Bind on specific endpoint
         */
        std::error_code bind(const endpoint &ep) noexcept;

        /**
         * Listen for request
         * @param backlog maximum queue length for pending connections
         */
        std::error_code listen() noexcept;
        std::error_code listen(int backlog) noexcept;

        /**
         * Accept new request and create a new socket
         */
        socket accept() noexcept;

    public:
        /**
         * Send data to connected host, usually used in tcp
         */
        ssize_t send(const void *data, std::size_t size, int flags = 0) noexcept;

        /**
         * Send data to specific host, usually used in udp
         */
        ssize_t send(const void *data, std::size_t size, const endpoint &ep, int flags = 0) noexcept;

        /**
         * Receive data from connected host, usually used in tcp
         */
        ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags = 0) noexcept;

        /**
         * Receive data from specific host, usually used in udp
         */
        ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, endpoint &ep, int flags = 0) noexcept;

    public:
        /**
         * Close the socket, the socket will disconnect immediately
         */
        std::error_code close() noexcept;

        /**
         * Stop send or receive, but socket is still connected
         */
        std::error_code shutdown(Shutdown flag) noexcept;

    public:
        /**
         * Local and remote endpoint
         */
        endpoint local() const noexcept;
        endpoint remote() const noexcept;

        /**
         * Non-blocking status
         */
        bool nonblocking() const noexcept;
        bool nonblocking(bool enable) noexcept;

        /**
         * Check socket is empty
         */
        bool empty() const noexcept;
        operator bool() const noexcept;

        /**
         * Native socket handle
         */
        socket_t native() const noexcept;

    private:
        socket(const socket&) = delete;
        socket& operator=(const socket&) = delete;

    private:
        socket_t _fd = socket_t();  // socket descriptor
    };
}