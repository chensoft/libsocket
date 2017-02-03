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
    class event : public basic_event
    {
    public:
        event();
        event(std::function<void ()> cb);
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
         * Native handle value
         */
        virtual handle_t native() const
        {
            return this->_handle.native();
        }

    public:
        /**
         * Bind & Emit callback
         */
        void bind(std::function<void ()> cb);
        void emit();

    protected:
        /**
         * At least one event has occurred
         */
        virtual void onEvent(reactor &loop, int type);

    private:
        bool _signaled = false;

        basic_handle _handle;
        std::function<void ()> _notify;

#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use pipe
        basic_handle _write;

#elif defined(_WIN32)

        // Windows, use udp
        basic_socket _write;

#else

        // Linux, use eventfd

#endif
    };
}