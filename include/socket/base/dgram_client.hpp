/**
 * Created by Jian Chen
 * @since  2016.11.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/dgram_socket.hpp>

namespace chen
{
    template <typename Address, typename Option>
    class dgram_client : public dgram_socket<Address, Option>
    {
    public:
        /**
         * Datagram client is connectionless, so we init socket here
         */
        dgram_client(int family, int protocol = 0)
        {
            this->_socket.reset(family, SOCK_DGRAM, protocol);
        }
    };
}