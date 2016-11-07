/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/bsd/basic_socket.hpp>

namespace chen
{
    class dgram_socket
    {
    protected:
        basic_socket _socket;
    };
}