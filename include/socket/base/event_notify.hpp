/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_event.hpp>

namespace chen
{
    class event_notify : public basic_event
    {
    public:
        event_notify();
        ~event_notify();

    public:
        /**
         * Change to signaled state
         */
        void set();

        /**
         * Reset to non-signaled state
         */
        void reset();

    private:
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use pipe
        handle_t _write = invalid_handle;

#else

        // Linux, use eventfd
        // Windows, use udp

#endif
    };
}