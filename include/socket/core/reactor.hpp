/**
 * Created by Jian Chen
 * @since  2016.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/core/event.hpp>
#include <socket/core/timer.hpp>
#include <socket/base/ev_event.hpp>
#include <socket/base/ev_timer.hpp>
#include <unordered_set>
#include <unordered_map>
#include <system_error>
#include <vector>
#include <queue>

namespace chen
{
    /**
     * Reactor implements a reactor event loop, note that only the stop method is thread-safe
     * @link https://en.wikipedia.org/wiki/Reactor_pattern
     */
    class reactor
    {
    public:
        /**
         * Event mode(LT means level-triggered, ET means edge-triggered)
         * ---------------------------------------------------------------------
         * Read(LT): event always occurs if the recv buffer has unread data
         * ---------------------------------------------------------------------
         * Read(ET): event occurs only when new data arrives, if you read part
         * of the data, event will no longer occurs unless new data arrives
         * ---------------------------------------------------------------------
         * Write(LT): event always occurs if the send buffer is not full
         * ---------------------------------------------------------------------
         * Write(ET, epoll): event occurs only when the state changes from
         * "cannot output" to "can output"
         * Write(ET, kqueue): event occurs after you send the data and the
         * send buffer is not full at that moment
         * ---------------------------------------------------------------------
         * @note since the socket has its own send buffer, you don't need to monitor
         * the write event from the start, usually you should call send() first, if
         * the method return EAGAIN then to wait for the write event occurs
         */
        static const int ModeRead;
        static const int ModeWrite;
        static const int ModeRW;

        /**
         * Event flag
         * @note Windows does not support edge-triggered, we simulate FlagOnce only
         */
        static const int FlagEdge;  // enable edge-triggered
        static const int FlagOnce;  // event occurs only once

    public:
        reactor();
        reactor(std::size_t count);  // the maximum events returned after polling, it will be ignored on Windows
        ~reactor();

    public:
        /**
         * Monitor event
         * @param mode ModeRead, ModeWrite and etc
         * @param flag FlagOnce, FlagEdge and etc
         */
        void set(basic_event *ptr, int mode, int flag);
        void set(timer *ptr);

        void set(ev_base *ptr, int mode, int flag);
        void set(ev_timer *ptr);

        /**
         * Delete event
         * @note this method will be called automatically when object destroyed, event is Closed or flag is FlagOnce
         */
        void del(basic_event *ptr);
        void del(timer *ptr);

        void del(ev_base *ptr);
        void del(ev_timer *ptr);

    public:
        /**
         * Run loop forever, unless user request to stop
         */
        void run();

        /**
         * Poll events and notify user via its callback
         * @return empty if at least one event is handled, operation_canceled if stop, interrupted if interrupt
         */
        std::error_code poll();

        /**
         * Poll events with a timeout, time accuracy varies with different systems
         * @param timeout use std::chrono::milliseconds, seconds and etc, return immediately if use zero as timeout value
         * @return empty if at least one event is handled, operation_canceled if stop, timed_out if timeout, interrupted if interrupt
         * @note it's useful when you want to use it in your own runloop, you can use zero timeout and call this method in every frame
         */
        std::error_code poll(std::chrono::nanoseconds timeout);

        /**
         * Post events to queue
         */
        void post(basic_event *ptr, int type);
        void post(timer *ptr, int type);

        /**
         * Stop the poll
         * @note you can call this method in callback or other thread to interrupt the poll
         */
        void stop();

    public:
        /**
         * Update timer manually
         * @note for advanced use only
         */
        std::chrono::nanoseconds update();

        /**
         * Gather events from backend
         * @note for advanced use only
         */
        std::error_code gather(std::chrono::nanoseconds timeout);

        /**
         * Notify events to user
         * @note for advanced use only
         */
        void notify();

    private:
        reactor(const reactor&) = delete;
        reactor& operator=(const reactor&) = delete;

    private:
        typedef struct
        {
            basic_event *ptr = nullptr;

            int mode = 0;
            int flag = 0;
            int type = 0;

            bool timer = false;
        } Data;

#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use kqueue
        typedef struct ::kevent event_t;

        handle_t _kqueue = invalid_handle;

#elif defined(__linux__)

        // Linux, use epoll
        typedef struct ::epoll_event event_t;

        handle_t _epoll = invalid_handle;

#else

        // Windows, use WSAPoll
        typedef struct ::pollfd event_t;

        chen::event _repoll;

#endif

        chen::event _wakeup;

        std::vector<event_t> _events;
        std::unordered_set<timer*> _timers;

        std::queue<Data> _pending;
        std::unordered_map<handle_t, Data> _handles;
    };
}