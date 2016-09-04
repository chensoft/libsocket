/**
 * Created by Jian Chen
 * @since  2016.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 * @link   http://man7.org/linux/man-pages/man7/socket.7.html
 */
#pragma once

#include <socket/base/base_socket.hpp>

namespace chen
{
    namespace option
    {
        // -----------------------------------------------------------------
        // basic
        class basic
        {
        public:
            static int optionInt(socket_t handle, int level, int name);
        };


        // -----------------------------------------------------------------
        // SO_ACCEPTCONN(read-only, determine whether the socket is a listening socket)
        class acceptconn : public basic
        {
        public:
            explicit acceptconn(const socket &sock);

        public:
            bool listening = false;
        };


        // -----------------------------------------------------------------
        // SO_REUSEADDR(allow local address reuse)
        // @caution on Windows SO_REUSEADDR means SO_REUSEADDR + SO_REUSEPORT
        class reuseaddr : public basic
        {
        public:
            /**
             * Construct by getsockopt or bool value
             */
            explicit reuseaddr(const socket &sock);
            explicit reuseaddr(bool val);

        public:
            /**
             * Call setsockopt to apply the value to socket
             */
            bool apply(socket &sock);

        public:
            bool enable = false;
        };


        // -----------------------------------------------------------------
        // SO_REUSEPORT(allow multiple sockets bind to the same port, you can listen on multiple threads)
        // @caution on Linux require kernel version 3.9+
        // @caution on Windows, I use SO_REUSEADDR to implement reuseport, because SO_REUSEADDR means SO_REUSEADDR + SO_REUSEPORT on Windows
        // @link http://stackoverflow.com/questions/14388706/socket-options-so-reuseaddr-and-so-reuseport-how-do-they-differ-do-they-mean-t/14388707#14388707
        class reuseport : public basic
        {
        public:
            explicit reuseport(const socket &sock);
            explicit reuseport(bool val);

        public:
            bool apply(socket &sock);

        public:
            bool enable = false;
        };


        // -----------------------------------------------------------------
        // SO_KEEPALIVE(enable keepalive on connection-oriented socket)
        class keepalive : public basic
        {
        public:
            explicit keepalive(const socket &sock);
            explicit keepalive(bool val);

        public:
            bool apply(socket &sock);

        public:
            bool enable = false;
        };


        // -----------------------------------------------------------------
        // SO_DONTROUTE(send packet to directly connected hosts, same as MSG_DONTROUTE flag)
        class dontroute : public basic
        {
        public:
            explicit dontroute(const socket &sock);
            explicit dontroute(bool val);

        public:
            bool apply(socket &sock);

        public:
            bool enable = false;
        };


        // -----------------------------------------------------------------
        // SO_BROADCAST(enable broadcast for datagram socket)
        class broadcast : public basic
        {
        public:
            explicit broadcast(const socket &sock);
            explicit broadcast(bool val);

        public:
            bool apply(socket &sock);

        public:
            bool enable = false;
        };


        // -----------------------------------------------------------------
        // SO_LINGER(close and shutdown will not return until data has been sent or timeout)
        class linger : public basic
        {
        public:
            explicit linger(const socket &sock);
            explicit linger(int onoff, int value);

        public:
            bool apply(socket &sock);

        public:
            int l_onoff  = 0;  // linger active
            int l_linger = 0;  // how many seconds to linger for
        };


        // -----------------------------------------------------------------
        // SO_SNDBUF(socket send buffer size)
        class sndbuf : public basic
        {
        public:
            explicit sndbuf(const socket &sock);
            explicit sndbuf(int val);

        public:
            bool apply(socket &sock);

        public:
            int size = 0;
        };


        // -----------------------------------------------------------------
        // SO_RCVBUF(socket receive buffer size)
        class rcvbuf : public basic
        {
        public:
            explicit rcvbuf(const socket &sock);
            explicit rcvbuf(int val);

        public:
            bool apply(socket &sock);

        public:
            int size = 0;
        };


        // -----------------------------------------------------------------
        // SO_SNDTIMEO(sending timeout)
        class sndtimeo : public basic
        {
        public:
            explicit sndtimeo(const socket &sock);
            explicit sndtimeo(struct timeval val);

        public:
            bool apply(socket &sock);

        public:
            struct timeval time{};
        };


        // -----------------------------------------------------------------
        // SO_RCVTIMEO(receiving timeout)
        class rcvtimeo : public basic
        {
        public:
            explicit rcvtimeo(const socket &sock);
            explicit rcvtimeo(struct timeval val);

        public:
            bool apply(socket &sock);

        public:
            struct timeval time{};
        };


        // -----------------------------------------------------------------
        // SO_ERROR(read-only, socket error)
        class error : public basic
        {
        public:
            explicit error(const socket &sock);

        public:
            std::error_code code;
        };


        // -----------------------------------------------------------------
        // SO_TYPE(read-only, socket type, SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
        class type : public basic
        {
        public:
            explicit type(const socket &sock);

        public:
            int val = 0;
        };
    }
}