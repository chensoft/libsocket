/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_socket.hpp>

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
         * Change to non-signaled state
         */
        void reset();

        /**
         * Check if signaled
         */
        bool signaled() const
        {
            return this->_signaled;
        }

        /**
         * Native event handle
         */
        basic_handle& handle();

    private:
        bool _signaled = false;

        basic_handle _handle;

#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use pipe
        handle_t _write = invalid_handle;

#elif defined(_WIN32)

        // Windows, use udp
        basic_socket _write;

#else

        // Linux, use eventfd

#endif
    };
}