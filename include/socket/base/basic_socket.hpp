/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "socket/base/basic_address.hpp"
#include "socket/base/ev_handle.hpp"
#include "socket/ip/ip_option.hpp"
#include <functional>

namespace chen
{
    /**
     * BSD socket wrapper, usually you don't need to use it directly, use
     * classes like tcp_client, tcp_server, udp_client, udp_server instead
     */
    class basic_socket : public ev_handle
    {
    public:
        enum class Shutdown {Read = 1, Write, Both};

        // cross-platform flags for recv and send
        static const int FlagOutOfBand;   // send out-of-band data on socket
        static const int FlagPeek;        // receive data without removing it from the queue
        static const int FlagDoNotRoute;  // send data to directly connected host
        static const int FlagWaitAll;     // block until the full request is satisfied or error

    public:
        /**
         * Empty socket
         */
        basic_socket(std::nullptr_t = nullptr) noexcept;

        /**
         * Construct by socket type
         * @param family AF_INET, AF_INET6, AF_UNIX...
         * @param type SOCK_STREAM, SOCK_DGRAM, SOCK_RAW...
         * @param protocol IPPROTO_TCP, IPPROTO_UDP or use zero directly
         */
        basic_socket(int family, int type, int protocol = 0);

        /**
         * Construct by socket handle directly
         * @note you can't use reset() if you construct only from fd, because we didn't know fd's family & protocol
         */
        basic_socket(handle_t fd) noexcept;
        basic_socket(handle_t fd, int family, int type, int protocol) noexcept;

        ~basic_socket() noexcept;

    public:
        /**
         * Reset socket by stored family, type and protocol
         * @note if you construct from a socket fd, this method will throw error because we didn't know how to create new socket
         */
        void reset();

        /**
         * Reset socket by the socket info
         */
        void reset(int family, int type, int protocol);

        /**
         * Reset socket by fd
         */
        void reset(handle_t fd) noexcept;
        void reset(handle_t fd, int family, int type, int protocol) noexcept;

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
        std::error_code listen(int backlog) noexcept;
        std::error_code listen() noexcept;

        /**
         * Accept new request and create a new socket
         * @note check to see if the result is valid before use it
         */
        std::error_code accept(basic_socket &s) noexcept;

    public:
        /**
         * Receive data from connected host, mainly used in stream socket
         */
        ssize_t recv(void *data, std::size_t size) noexcept;
        ssize_t recv(void *data, std::size_t size, int flags) noexcept;

        /**
         * Receive data from specific host, used in datagram socket
         */
        ssize_t recvfrom(void *data, std::size_t size) noexcept;
        ssize_t recvfrom(void *data, std::size_t size, basic_address &addr) noexcept;
        ssize_t recvfrom(void *data, std::size_t size, basic_address &addr, int flags) noexcept;

        /**
         * Send data to connected host, mainly used in stream socket
         */
        ssize_t send(const void *data, std::size_t size) noexcept;
        ssize_t send(const void *data, std::size_t size, int flags) noexcept;

        /**
         * Send data to specific host, used in datagram socket
         */
        ssize_t sendto(const void *data, std::size_t size, const basic_address &addr) noexcept;
        ssize_t sendto(const void *data, std::size_t size, const basic_address &addr, int flags) noexcept;

    public:
        /**
         * Stop send or receive, but socket is still valid
         */
        void shutdown(Shutdown type = Shutdown::Both) noexcept;

        /**
         * Close the socket, the socket will disconnect immediately
         */
        virtual void close() noexcept override;

    public:
        /**
         * Peer & Local address
         */
        template <typename R>
        R peer() const noexcept
        {
            R ret;
            this->peer(ret);
            return ret;
        }

        template <typename R>
        R sock() const noexcept
        {
            R ret;
            this->sock(ret);
            return ret;
        }

        std::error_code peer(basic_address &addr) const noexcept;
        std::error_code sock(basic_address &addr) const noexcept;

        /**
         * Non-blocking mode
         */
        std::error_code nonblocking(bool enable) noexcept;

        /**
         * Check socket is valid
         * @note note that 0 is a valid file descriptor number
         */
        bool valid() const noexcept;
        operator bool() const noexcept;

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

    public:
        /**
         * Attach callback
         */
        void attach(std::function<void (int type)> cb) noexcept;

    protected:
        /**
         * At least one event has occurred
         * @note you should read the rest of the data even if you received the Closed
         * event, server may send last message and then close the connection immediately
         * the backend may report Readable & Closed event or only report the Closed event
         */
        virtual void onEvent(int type) override;

    private:
        // used for reset socket
        // only type is valid if you construct from a socket descriptor
        int _family   = 0;
        int _type     = 0;
        int _protocol = 0;

        std::function<void (int type)> _notify;
    };
}
