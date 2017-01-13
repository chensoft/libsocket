/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_handle.hpp>

namespace chen
{
    class event
    {
    public:
        event();
        ~event();

    public:
        /**
         * Change to signaled state
         */
        void set();

        /**
         * Reset to non-signaled state
         */
        void reset();

        /**
         * Native event handle
         */
        basic_handle& handle();

    private:
        basic_handle _handle;

#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use pipe
        handle_t _write = invalid_handle;

#else

        // Linux, use eventfd
        // Windows, use udp

#endif
    };
}