/**
 * Created by Jian Chen
 * @since  2016.11.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_socket.hpp>

namespace chen
{
    template <typename T>
    class dgram_socket : public basic_socket<T>
    {
    };
}