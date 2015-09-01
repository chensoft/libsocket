/**
 * Created by Jian Chen
 * @since  2015.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "udp_socket.h"
#include "udp_packet.h"
#include "../../sync/threadpool.h"
#include <functional>

// todo just for test
namespace chen
{
    namespace udp
    {
        class server : public socket
        {
        public:
            typedef std::function<void (const chen::udp::packet &pkt)> recv_type;

        public:
            server(recv_type callback);

        public:
            virtual void run(const std::string &addr, std::uint16_t port);

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