/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <cstdint>
#include <string>

namespace chen
{
    namespace udp
    {
        class socket
        {
        public:
            socket();
            virtual ~socket() = 0;

        public:
            virtual std::size_t send(const std::uint8_t *data, std::size_t size, const std::string &addr, std::uint16_t port);

            virtual std::size_t recv(std::uint8_t *data, std::size_t size, std::string &addr, std::uint16_t &port);

            virtual void close();

        protected:
            virtual void build();

        private:
            socket(const socket &) = delete;
            socket& operator=(const socket &) = delete;

        protected:
            class impl;
            std::unique_ptr<impl> _impl;
        };
    }
}