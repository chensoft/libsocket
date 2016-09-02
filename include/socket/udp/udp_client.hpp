/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/udp/udp_basic.hpp>

namespace chen
{
    namespace udp
    {
        class client : public basic
        {
        public:
            /**
             * Send data to specific host
             */
            using basic::send;
            ssize_t send(const std::vector<std::uint8_t> &data, const endpoint &ep, int flags = 0);

            /**
             * Receive data from specific host
             */
            using basic::recv;
            std::vector<std::uint8_t> recv(std::size_t size, endpoint &ep, int flags = 0);

        public:
            /**
             * Stored host and port
             */
            std::string host() const;
            std::uint16_t port() const;

        private:
            std::string _host;
            std::uint16_t _port = 0;
        };
    }
}