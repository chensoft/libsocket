/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/inet/inet_address.hpp>
#include <socket/base/bsd_socket.hpp>
#include <socket/core/runloop.hpp>

namespace chen
{
    /**
     * Base class for TCP & UDP socket
     */
    class inet_socket
    {
    public:
        /**
         * Peer & Local address
         */
        inet_address peer() const;
        inet_address sock() const;

        /**
         * Check socket is valid
         * @attention zero is a valid file descriptor
         */
        bool valid() const;
        operator bool() const;

    protected:
        /**
         * Non-blocking mode
         */
        std::error_code nonblocking(bool enable);

        /**
         * Reset socket
         */
        void reset();
        void reset(bsd_socket &&s);
        void reset(ip_address::Type family, int type);

    protected:
        bsd_socket _socket;
    };
}