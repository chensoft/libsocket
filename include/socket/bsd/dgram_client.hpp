/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/dgram_socket.hpp>
#include <socket/sys/runloop.hpp>

namespace chen
{
    class dgram_client : public dgram_socket
    {
    public:
        /**
         * Datagram client is connectionless, so we init socket
         * in constructor, we also make the socket nonblocking
         */
        dgram_client(runloop &loop, int family, int protocol = 0);
        ~dgram_client();

    protected:
        /**
         * Event handler
         */
        void onReadable();
        void onWritable();
        void onEvent(runloop::Event type);

    protected:
        runloop &_runloop;
    };
}