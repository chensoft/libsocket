/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>
#include <socket/bsd/bsd_socket.hpp>

namespace chen
{
    namespace net
    {
        class socket
        {
        public:
            socket(bsd::socket &&s);
            socket(ip::address::Type family, int type);

        public:
            /**
             * Sock & Peer endpoint
             */
            net::endpoint sock() const;
            net::endpoint peer() const;

            /**
             * Non-blocking mode
             */
            std::error_code nonblocking(bool enable);

            /**
             * Check socket is valid
             */
            bool valid() const;
            operator bool() const;

        protected:
            bsd::socket _handle;
        };
    }
}