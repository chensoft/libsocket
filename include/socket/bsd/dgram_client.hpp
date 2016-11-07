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
         * Read a packet from remote
         * the read callback will be invoked if successful
         * @param size the desired read length, actual size will be less or equal than this value
         */
        void read(std::size_t size);

        /**
         * Write a packet to remote
         * the write callback will be invoked if successful
         */
        void write(const char *text, const basic_address &addr);
        void write(const std::string &text, const basic_address &addr);
        void write(const void *data, std::size_t size, const basic_address &addr);

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
         * Receive data
         */
        void receive();

        /**
         * Event handler
         */
        void onReadable();
        void onWritable();
        void onEnded();
        void onEvent(runloop::Event type);

    protected:
        runloop &_runloop;

        std::size_t           _reading = 0;
        dgram_client_handler *_handler = nullptr;

        std::vector<dgram_packet> _buf_write;
    };
}