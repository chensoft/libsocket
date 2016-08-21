/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_socket.hpp>
#include <socket/udp/udp_define.hpp>

namespace chen
{
    namespace udp
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
             * Local endpoint
             */
            net::endpoint local() const;
        };
    }
}