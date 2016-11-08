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
    class dgram_server : public dgram_socket
    {
    public:
        /**
         * Construct by the address
         */
        dgram_server(runloop &loop, const basic_address &addr, int protocol = 0);
        ~dgram_server();

    public:
        void listen(int backlog = 0);
        void close();

    protected:
        runloop &_runloop;
    };
}