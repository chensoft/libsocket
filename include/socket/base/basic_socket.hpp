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
    /**
     * The basic class for dgram_socket, stream_socket and more
     */
    template <typename T>
    class basic_socket
    {
    public:
        /**
         * Requirements for address:
         * -:) must have a constructor which accept bsd_address as its param
         * -:) must define bsd_address conversion operator, see inet_address
         */
        typedef typename T::address_type address_type;

        /**
         * Requirements for option:
         * -:) must derived from bsd_option and accept bsd_socket& as its param
         */
        typedef typename T::option_type option_type;

    public:
        /**
         * Peer & Local address
         */
        address_type peer() const noexcept
        {
            return this->_socket.peer();
        }

        address_type sock() const noexcept
        {
            return this->_socket.sock();
        }

        /**
         * Non-blocking mode
         */
        std::error_code nonblocking(bool enable) noexcept
        {
            return this->_socket.nonblocking(enable);
        }

        /**
         * Get/Set socket option
         * Usage:
         * >> auto opt = socket.option();
         * >> opt.reuseaddr(true);           // set option
         * >> bool reuse = opt.reuseaddr();  // get option
         */
        option_type option() noexcept
        {
            return option_type(*this);
        }

        /**
         * Check socket is valid
         * @attention zero is a valid file descriptor
         */
        bool valid() const noexcept
        {
            return this->_socket.valid();
        }

        operator bool() const noexcept
        {
            return this->_socket;
        }

        /**
         * Native socket handle
         */
        socket_t native() const noexcept
        {
            return this->_socket.native();
        }

        /**
         * Available bytes to read without blocking
         */
        std::size_t available() const noexcept
        {
            return this->_socket.available();
        }

        /**
         * Native type info
         */
        int family() const noexcept
        {
            return this->_socket.family();
        }

        int type() const noexcept
        {
            return this->_socket.type();
        }

        int protocol() const noexcept
        {
            return this->_socket.protocol();
        }

    protected:
        bsd_socket _socket;
    };
}