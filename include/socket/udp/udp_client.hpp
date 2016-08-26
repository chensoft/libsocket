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
            ssize_t send(const void *data, std::size_t size, int flags, const net::endpoint &ep) noexcept;
            ssize_t send(const std::vector<std::uint8_t> &data, int flags, const net::endpoint &ep) noexcept;

            /**
             * Receive data from specific host
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags, net::endpoint &ep) noexcept;
            std::vector<std::uint8_t> recv(std::size_t size, int flags, net::endpoint &ep) noexcept;

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