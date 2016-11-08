/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/dgram_packet.hpp>
#include <socket/bsd/dgram_socket.hpp>
#include <socket/sys/runloop.hpp>
#include <string>

namespace chen
{
    class dgram_client_handler;

    class dgram_client : public dgram_socket
    {
    public:
        /**
         * Datagram client is connectionless, so we init socket
         * in constructor, we also make the socket nonblocking
         */
        dgram_client(runloop &loop, int family, int protocol = 0);
        ~dgram_client();

    public:
        /**
         * Attach & Detach event handler
         */
        void attach(dgram_client_handler *handler);
        void detach();

    protected:
        /**
         * Notify events
         */
        void notify(std::vector<std::uint8_t> &&data, const chen::basic_address &addr);
        void notify(std::size_t size, const chen::basic_address &addr);

        /**
         * Event handler
         */
        void onReadable();
        void onWritable();
        void onEnded();
        void onEvent(runloop::Event type);

    protected:
        runloop &_runloop;

        dgram_client_handler *_handler = nullptr;

        std::vector<dgram_packet> _buf_write;
    };
}