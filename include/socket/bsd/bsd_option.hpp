/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>
#include <system_error>

namespace chen
{
    namespace bsd
    {
        class socket;

        class option
        {
        public:
            option(socket_t fd);

        public:
            /**
             * SO_ACCEPTCONN(read-only, determine whether the socket is a listening socket)
             */
            bool acceptconn() const;

            /**
             * SO_REUSEADDR(allow local address reuse)
             * @notice on Windows SO_REUSEADDR means SO_REUSEADDR + SO_REUSEPORT
             */
            bool reuseaddr() const;
            bool reuseaddr(bool val);

            /**
             * SO_REUSEPORT(allow multiple sockets bind to the same port, you can listen on multiple threads)
             * @notice on Linux require kernel version 3.9+
             * @notice on Windows, I use SO_REUSEADDR to implement reuseport, because SO_REUSEADDR means SO_REUSEADDR + SO_REUSEPORT on Windows
             * @link http://stackoverflow.com/questions/14388706/socket-options-so-reuseaddr-and-so-reuseport-how-do-they-differ-do-they-mean-t/14388707#14388707
             */
            bool reuseport() const;
            bool reuseport(bool val);

            /**
             * SO_KEEPALIVE(enable keepalive on connection-oriented socket)
             */
            bool keepalive() const;
            bool keepalive(bool val);

            /**
             * SO_DONTROUTE(send packet to directly connected hosts, same as MSG_DONTROUTE flag)
             */
            bool dontroute() const;
            bool dontroute(bool val);

            /**
             * SO_BROADCAST(enable broadcast for datagram socket)
             */
            bool broadcast() const;
            bool broadcast(bool val);

            /**
             * SO_LINGER(close and shutdown will not return until data has been sent or timeout)
             */
            struct ::linger linger() const;
            bool linger(int onoff, int value);

            /**
             * SO_SNDBUF(socket send buffer size)
             */
            int sndbuf() const;
            bool sndbuf(int val);

            /**
             * SO_RCVBUF(socket receive buffer size)
             */
            int rcvbuf() const;
            bool rcvbuf(int val);

            /**
             * SO_SNDTIMEO(sending timeout)
             */
            struct ::timeval sndtimeo() const;
            bool sndtimeo(int sec, int usec);

            /**
             * SO_RCVTIMEO(receiving timeout)
             */
            struct ::timeval rcvtimeo() const;
            bool rcvtimeo(int sec, int usec);

            /**
             * SO_ERROR(read-only, socket error)
             */
            std::error_code error() const;

            /**
             * SO_TYPE(read-only, socket type, SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
             */
            int type() const;

        private:
            /**
             * Helper
             */
            int intVal(int level, int name) const;

        private:
            socket_t _fd;
        };
    }
}