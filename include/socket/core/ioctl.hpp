/**
 * Created by Jian Chen
 * @since  2017.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>
#include <system_error>
#include <string>

namespace chen
{
    class ioctl
    {
    public:
        /**
         * Add or remove close on exec
         * @note no effect on Windows
         */
        static std::error_code cloexec(handle_t fd, bool enable);

        /**
         * Available bytes to read
         */
        static std::size_t available(handle_t fd);

        /**
         * Enable or disable non-blocking mode
         */
        static std::error_code nonblocking(handle_t fd, bool enable);
    };
}