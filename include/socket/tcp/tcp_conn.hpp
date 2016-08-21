/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_basic.hpp>

namespace chen
{
    namespace tcp
    {
        class conn : public basic
        {
        public:
            /**
             * Send data to connected host, usually used in tcp
             * todo make flags to enum class
             * todo add deadline
             */
            ssize_t send(const void *data, std::size_t size, int flags);
            ssize_t send(const std::vector<std::uint8_t> &data, int flags);

            /**
             * Receive data from connected host, usually used in tcp
             */
            ssize_t recv(std::vector<std::uint8_t> &out, std::size_t size, int flags);
            std::vector<std::uint8_t> recv(std::size_t size, int flags);

        public:
            /**
             * Close the connection
             */
            bool close();
        };
    }
}