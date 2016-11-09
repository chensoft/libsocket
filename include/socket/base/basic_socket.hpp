/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/bsd_socket.hpp>

namespace chen
{
    template <typename T>
    class basic_socket
    {
    public:
        typedef typename T::address_type address_type;
        typedef typename T::option_type option_type;

    public:
        /**
         * Peer & Local address
         */
        address_type peer() const noexcept;
        address_type sock() const noexcept;

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
        option_type option() noexcept;

        /**
         * Check socket is valid
         * @attention zero is a valid file descriptor
         */
        bool valid() const noexcept;
        operator bool() const noexcept;

        /**
         * Native socket handle
         */
        socket_t native() const noexcept;

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

    protected:
        bsd_socket _socket;
    };
}

#include <socket/base/basic_socket.ipp>