/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/prot/inet/net_endpoint.hpp>
#include <socket/prot/inet/net_runloop.hpp>
#include <socket/base/bsd_socket.hpp>

namespace chen
{
    namespace net
    {
        class socket
        {
        public:
            /**
             * Peer & Local endpoint
             */
            net::endpoint peer() const;
            net::endpoint sock() const;

            /**
             * Check socket is valid
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
            void reset(bsd::socket &&s);
            void reset(ip::address::Type family, int type);

        protected:
            bsd::socket _socket;
        };
    }
}