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

            virtual ~socket() = 0;

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

            /**
             * Native type info
             */
            int family() const;
            int type() const;
            int protocol() const;

            // todo remove virtual and these methods
//        protected:
//            /**
//             * Event callbacks
//             * :-) accept : used by server socket to accepting new connection
//             * :-) read   : the data has arrived
//             * :-) write  : data has been sent out
//             * :-) end    : connection refused, disconnect or other error
//             * @attention ep is only valid when socket's type is udp, otherwise it's nullptr
//             */
//            virtual void onAccept(bsd::socket s, net::endpoint ep);
//            virtual void onRead(std::vector<std::uint8_t> data, net::endpoint ep, std::error_code error);
//            virtual void onWrite(std::size_t size, net::endpoint ep, std::error_code error);
//            virtual void onEnd();

        protected:
            bsd::socket _handle;
        };
    }
}