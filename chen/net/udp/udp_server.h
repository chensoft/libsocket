/**
 * Created by Jian Chen
 * @since  2015.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "udp_socket.h"
#include "udp_packet.h"
#include <chen/sync/threadpool.h>
#include <functional>

namespace chen
{
    namespace udp
    {
        class server : public socket
        {
        public:
            typedef std::function<void (chen::udp::socket *so, const chen::udp::packet &pkt)> recv_type;

        public:
            server(recv_type callback);

        public:
            virtual void run(const std::string &addr, std::uint16_t port, std::size_t size = 65507);

        public:
            virtual std::size_t send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port);

        protected:
            virtual void bind(const std::string &addr, std::uint16_t port);

        protected:
            recv_type _func;
            threadpool _pool;
        };
    }
}