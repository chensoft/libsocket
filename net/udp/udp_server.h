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
        class server : public socket
        {
        public:
            server();

        public:
            virtual void bind(const std::string &addr, std::uint16_t port);
        };
    }
}