/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>

namespace chen
{
    class reactor;

    class basic_event
    {
    public:
        /**
         * Event type
         * ---------------------------------------------------------------------
         * Readable: you can read data from remote
         * ---------------------------------------------------------------------
         * Writable: you can write data to remote
         * ---------------------------------------------------------------------
         * Closed: socket disconnected or connection refused
         * ---------------------------------------------------------------------
         * @note in epoll, Closed event is always be monitored, in kqueue and poll
         * you must monitor the Readable event if you want to know the Closed event
         * ---------------------------------------------------------------------
         * @note you should read the rest of the data even if you received the Closed
         * event, server may send last message and then close the connection immediately
         * the backend may report Readable & Closed event or only report the Closed event
         */
        static const int Readable;
        static const int Writable;
        static const int Closed;

    public:
        basic_event() = default;
        virtual ~basic_event() = default;

    public:
        /**
         * Native handle value
         */
        virtual handle_t native() const = 0;

    protected:
        friend class reactor;

        /**
         * Notify user that at least one event has occurred
         * @note use bitwise and to check the event type, e.g: if (type & Readable)
         */
        virtual void onEvent(reactor &loop, int type) = 0;

    private:
        /**
         * Disable copy & move, if you want to store object in container
         * you can use smart pointer like std::unique_ptr<basic_event>
         */
        basic_event(const basic_event&) = delete;
        basic_event& operator=(const basic_event&) = delete;
    };
}