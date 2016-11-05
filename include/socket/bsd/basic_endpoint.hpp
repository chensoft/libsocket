/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>

namespace chen
{
    /**
     * basic socket address wrapper, you don't need to use it directly
     */
    class basic_endpoint
    {
    public:
        socklen_t size = sizeof(addr);  // use sockaddr_storage's size as initialization value
        struct ::sockaddr_storage addr{};
    };
}