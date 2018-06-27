/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include "socket/base/basic_socket.hpp"
#include <memory>

namespace chen
{
    class ev_event_impl;

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
        virtual void onEvent(int type) override;

    private:
        bool _signaled = false;
        std::function<void ()> _notify;

#ifdef __linux__
        // Linux, use eventfd
#else
        // Unix, use pipe
        // Windows, use socket
        std::unique_ptr<ev_event_impl> _impl;
#endif
    };
}