/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "socket/config.hpp"
#include <system_error>
#include <string>

namespace chen
{
    class basic_option
    {
    public:
        /**
         * SO_DEBUG(turn on debugging info recording)
         */
        static bool debug(handle_t fd);
        static bool debug(handle_t fd, bool val);

        /**
         * SO_REUSEADDR(allow local address reuse)
         * @note on Windows SO_REUSEADDR means SO_REUSEADDR + SO_REUSEPORT
         */
        static bool reuseaddr(handle_t fd);
        static bool reuseaddr(handle_t fd, bool val);

        /**
         * SO_REUSEPORT(allow multiple sockets bind to the same port, you can listen on multiple threads)
         * @note on Linux require kernel version 3.9+
         * @note on Windows, you can use SO_REUSEADDR instead of SO_REUSEPORT, because SO_REUSEADDR means SO_REUSEADDR + SO_REUSEPORT on Windows
         * @link http://stackoverflow.com/questions/14388706/socket-options-so-reuseaddr-and-so-reuseport-how-do-they-differ-do-they-mean-t/14388707#14388707
         */
        static bool reuseport(handle_t fd);
        static bool reuseport(handle_t fd, bool val);

        /**
         * SO_KEEPALIVE(enable keepalive on connection-oriented socket)
         * @note keepalive option works only for the server side
         * @note keepalive will detect only when there is no data transfer
         * @note keepalive may not be accurate because a proxy server may exist
         */
        static bool keepalive(handle_t fd);
        static bool keepalive(handle_t fd, bool val);

        /**
         * SO_DONTROUTE(send packet to directly connected hosts, same as MSG_DONTROUTE flag)
         */
        static bool dontroute(handle_t fd);
        static bool dontroute(handle_t fd, bool val);

        /**
         * SO_BROADCAST(enable broadcast for datagram socket)
         */
        static bool broadcast(handle_t fd);
        static bool broadcast(handle_t fd, bool val);

//        /**
//         * SO_LINGER(close and shutdown will not return until data has been sent or timeout)
//         */
//        static struct ::linger linger(handle_t fd);
//        static bool linger(handle_t fd, int onoff, int value);
//        static bool linger(handle_t fd, const struct ::linger &val);

        /**
         * SO_OOBINLINE(leave received OOB data in line)
         */
        static bool oobinline(handle_t fd);
        static bool oobinline(handle_t fd, bool val);

        /**
         * SO_SNDBUF(socket send buffer size)
         */
        static int sndbuf(handle_t fd);
        static bool sndbuf(handle_t fd, int val);

        /**
         * SO_RCVBUF(socket recv buffer size)
         */
        static int rcvbuf(handle_t fd);
        static bool rcvbuf(handle_t fd, int val);

        /**
         * SO_SNDLOWAT(send low-water mark)
         */
        static int sndlowat(handle_t fd);
        static bool sndlowat(handle_t fd, int val);

        /**
         * SO_RCVLOWAT(receive low-water mark)
         */
        static int rcvlowat(handle_t fd);
        static bool rcvlowat(handle_t fd, int val);

//        /**
//         * SO_SNDTIMEO(sending timeout)
//         */
//        static struct ::timeval sndtimeo(handle_t fd);
//        static bool sndtimeo(handle_t fd, int sec, int usec);
//        static bool sndtimeo(handle_t fd, const struct ::timeval &time);
//
//        /**
//         * SO_RCVTIMEO(receiving timeout)
//         */
//        static struct ::timeval rcvtimeo(handle_t fd);
//        static bool rcvtimeo(handle_t fd, int sec, int usec);
//        static bool rcvtimeo(handle_t fd, const struct ::timeval &time);

        /**
         * SO_ERROR(read-only, socket error)
         */
        static std::error_code error(handle_t fd);

        /**
         * SO_TYPE(read-only, socket type, SOCK_STREAM, SOCK_DGRAM, SOCK_RAW)
         */
        static int type(handle_t fd);

    public:
        /**
         * Helper
         */
        static int get(handle_t fd, int level, int name);
        static bool set(handle_t fd, int level, int name, int val);
    };
}