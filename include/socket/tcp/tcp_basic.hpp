/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>

namespace chen
{
    namespace tcp
    {
        class basic
        {
        public:
            /**
             * Last error info, compare it with std::errc enum class
             * e.g: socket.error() == std::errc::permission_denied
             */
            std::error_code error() const;

            /**
             * Local and remote endpoint
             */
            net::endpoint local() const;
            net::endpoint remote() const;

        protected:
            net::socket _socket = nullptr;
        };
    }
}