/**
 * Created by Jian Chen
 * @since  2015.08.22
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include "udp_socket.h"

namespace chen
{
    namespace udp
    {
        class client : public socket
        {
        public:
            client();

        public:
            virtual std::size_t send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port) override;

            virtual std::size_t recv(std::uint8_t *data, std::size_t size, std::string &addr, std::uint16_t &port) override;
        };
    }
}