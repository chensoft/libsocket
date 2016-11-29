/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_address.hpp>
#include <socket/base/basic_option.hpp>

namespace chen
{
    /**
     * BSD socket wrapper, usually you don't need to use it directly
     * use tcp_client, tcp_server, udp_client, udp_server instead
     */
    class basic_socket
    {
    public:
        enum class Shutdown {Read = 1, Write, Both};

        // cross-platform flags for recv and send
        static constexpr int FlagOutOfBand  = MSG_OOB;        // send out-of-band data on socket
        static constexpr int FlagPeek       = MSG_PEEK;       // receive data without removing it from the queue
        static constexpr int FlagDoNotRoute = MSG_DONTROUTE;  // send data to directly connected host
        static constexpr int FlagWaitAll    = MSG_WAITALL;    // block until the full request is satisfied or error

    public:
        /**
         * Empty socket
         */
        basic_socket(std::nullptr_t = nullptr) noexcept;

        /**
         * Construct by socket handle directly
         * @attention you can't use reset() if you construct only from fd, because we didn't know fd's family & protocol
         */
        basic_socket(handle_t fd) noexcept;
        basic_socket(handle_t fd, int family, int type, int protocol) noexcept;

        /**
         * Construct by socket type
         * @param family AF_INET, AF_INET6, AF_UNIX...
         * @param type SOCK_STREAM, SOCK_DGRAM, SOCK_RAW...
         * @param protocol IPPROTO_TCP, IPPROTO_UDP or use zero directly
         */
        basic_socket(int family, int type, int protocol = 0);

        basic_socket(basic_socket &&o) noexcept;
        basic_socket& operator=(basic_socket &&o) noexcept;

        ~basic_socket() noexcept;

    public:
        /**
         * Reset socket by stored family, type and protocol
         * @attention if you construct from a socket fd, this method will throw error because it didn't know how to create new socket
         */
        void reset();

        /**
         * Reset socket by fd or info
         */
        void reset(handle_t fd) noexcept;
        void reset(int family, int type, int protocol = 0);

    public:
        /**
         * Connect to remote address
         */
        std::error_code connect(const basic_address &addr) noexcept;

        /**
         * Bind on specific address
         */
        std::error_code bind(const basic_address &addr) noexcept;

        /**
         * Listen for request
         * @param backlog maximum queue length for pending connections
         */
        std::error_code listen(int backlog = 0) noexcept;

        /**
         * Accept new request and create a new socket
         * @attention check to see if the result is valid before use it
         */
        basic_socket accept() noexcept;
        basic_socket accept(basic_address &addr) noexcept;

    public:
        /**
         * Receive data from connected host, used in stream socket
         */
        ssize_t recv(void *data, std::size_t size, int flags = 0) noexcept;

        /**
         * Receive data from specific host, used in datagram socket
         */
        ssize_t recvfrom(void *data, std::size_t size, basic_address &addr, int flags = 0) noexcept;

        /**
         * Send data to connected host, used in stream socket
         */
        ssize_t send(const void *data, std::size_t size, int flags = 0) noexcept;

        /**
         * Send data to specific host, used in datagram socket
         */
        ssize_t sendto(const void *data, std::size_t size, const basic_address &addr, int flags = 0) noexcept;

    public:
        /**
         * Stop send or receive, but socket is still valid
         */
        void shutdown(Shutdown type = Shutdown::Both) noexcept;

        /**
         * Close the socket, the socket will disconnect immediately
         */
        void close() noexcept;

        /**
         * Detach the socket handle, you must close the handle manually
         * @attention the family, type and protocol info is preserved
         * @attention this method is dangerous, you may leak the handle if you forget to close it
         */
        handle_t detach() noexcept;

    public:
        /**
         * Peer & Local address
         */
        basic_address peer() const noexcept;
        basic_address sock() const noexcept;

        /**
         * Non-blocking mode
         */
        std::error_code nonblocking(bool enable) noexcept;

        /**
         * Get/Set socket option
         * Usage:
         * >> auto opt = socket.option();
         * >> opt.reuseaddr(true);           // set option
         * >> bool reuse = opt.reuseaddr();  // get option
         */
        basic_option option() noexcept;

        /**
         * Check socket is valid
         * @attention zero is a valid file descriptor
         */
        bool valid() const noexcept;
        operator bool() const noexcept;

        /**
         * Native socket handle
         */
        handle_t native() const noexcept;

        /**
         * Available bytes to read without blocking
         */
        std::size_t available() const noexcept;

        /**
         * Native type info
         */
        int family() const noexcept;
        int type() const noexcept;
        int protocol() const noexcept;

    private:
        /**
         * Disable copy
         * if you want to store socket in container
         * use smart pointer like std::unique_ptr<basic_socket>
         */
        basic_socket(const basic_socket&) = delete;
        basic_socket& operator=(const basic_socket&) = delete;

    private:
        handle_t _fd = invalid_handle;  // socket descriptor

        // used for reset socket
        // only type is valid if you construct from a socket descriptor
        int _family   = 0;
        int _type     = 0;
        int _protocol = 0;
    };
}