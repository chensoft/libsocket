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
         * Reset to non-signaled state
         */
        void reset();

        /**
         * Native event handle
         * you can use it in reactor, usually you register it with ModeRead, if
         * Readable event occurs then you can reset the state and do your jobs
         */
        handle_t native() const;

    private:
        event(const event&) = delete;
        event& operator=(const event&) = delete;

    private:
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use pipe
        handle_t _pp[2]{invalid_handle, invalid_handle};

#elif defined(__linux__)

        // Linux, use eventfd
        handle_t _fd = invalid_handle;

#else

        // Windows, use udp
        basic_socket _socket;  // WSAPoll only support SOCKET

#endif
    };
}