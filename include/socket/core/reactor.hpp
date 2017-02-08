/**
 * Created by Jian Chen
 * @since  2016.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/ev_event.hpp>
#include <socket/base/ev_timer.hpp>
#include <unordered_map>
#include <unordered_set>
#include <system_error>
#include <vector>
#include <queue>
#include <set>

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
        void set(ev_base *ptr, int mode, int flag);
        void set(ev_timer *ptr, std::chrono::high_resolution_clock::time_point init = std::chrono::high_resolution_clock::now());

        /**
         * Delete event
         * @note this method will be called automatically when object destroyed, event is Closed or flag is FlagOnce
         */
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
        void post(ev_base *ptr, int type);
        void post(ev_timer *ptr, int type);

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
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use kqueue
        typedef struct ::kevent event_t;

        handle_t _backend = invalid_handle;
        std::unordered_set<ev_base*> _objects;

#elif defined(__linux__)

        // Linux, use epoll
        typedef struct ::epoll_event event_t;

        handle_t _backend = invalid_handle;
        std::unordered_set<ev_base*> _objects;

#else

        // Windows, use WSAPoll
        typedef struct ::pollfd event_t;

        ev_event _repoll;
        std::unordered_map<handle_t, ev_base*> _objects;

#endif

        ev_event _wakeup;
        std::multiset<ev_timer*, ev_timer::Compare> _timers;

        std::vector<event_t> _cache;
        std::queue<std::pair<ev_base*, int>> _queue;
    };
}