/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/ev_handle.hpp>
#include <functional>

namespace chen
{
    class ev_event: public ev_handle
    {
    public:
        ev_event(std::function<void ()> cb = nullptr);
        ~ev_event();

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

    public:
        /**
         * Attach callback
         */
        void attach(std::function<void ()> cb);

    protected:
        /**
         * At least one event has occurred
         */
        virtual void onEvent(int type);

    private:
        bool _signaled = false;
        std::function<void ()> _notify;

#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use pipe
        handle_t _write;

#elif defined(_WIN32)

        // Windows, use udp
        basic_socket _write;

#else

        // Linux, use eventfd

#endif
    };
}