/**
 * Created by Jian Chen
 * @since  2016.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/core/event.hpp>
#include <socket/core/timer.hpp>
#include <unordered_set>
#include <unordered_map>
#include <system_error>
#include <vector>
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
        /**
         * Event type
         * ---------------------------------------------------------------------
         * Readable: you can read data from remote
         * ---------------------------------------------------------------------
         * Writable: you can write data to remote
         * ---------------------------------------------------------------------
         * Closed: socket disconnected or connection refused
         * ---------------------------------------------------------------------
         * @note in epoll, closed event is always be monitored, in kqueue and poll
         * you must monitor the read event if you want to know the closed event
         * ---------------------------------------------------------------------
         * @note you should read the rest of the data even if you received the closed
         * event, server may send last message and then close the connection immediately
         * the backend may report readable & closed event or only report the closed event
         */
        static const int Readable;
        static const int Writable;
        static const int Closed;

    public:
        reactor();
        reactor(short count);  // max events returned after a polling, it will be ignored on Windows
        ~reactor();

    public:
        /**
         * Monitor event
         * @param cb you can use std::bind to bind custom param
         * @param mode ModeRead, ModeWrite and etc
         * @param flag FlagOnce, FlagEdge and etc
         * @note use bitwise and to check the event type, e.g: if (type & Readable)
         */
        void set(basic_handle *ptr, std::function<void (int type)> cb, int mode, int flag);

        /**
         * Specific methods for socket, event, timer, and etc
         */
        void set(basic_socket *ptr, std::function<void (int type)> cb, int mode, int flag);
        void set(event *ptr, std::function<void ()> cb, int flag);
        void set(timer *ptr, std::function<void ()> cb);

        /**
         * Delete event
         * @note this method will be called automatically when object destroyed, event is Closed or flag is FlagOnce
         */
        void del(basic_handle *ptr);
        void del(basic_socket *ptr);
        void del(event *ptr);
        void del(timer *ptr);

    public:
        /**
         * Run loop forever, unless user request to stop
         */
        void run();

        /**
         * Poll events and notify user via its callback
         * @return empty if an event is handled, operation_canceled if stop, interrupted if interrupt
         */
        std::error_code poll();

        /**
         * Poll events with a timeout, time accuracy varies with different systems
         * @param timeout use std::chrono::milliseconds, seconds and etc, return immediately if use zero as duration value
         * @return empty if an event is handled, operation_canceled if stop, timed_out if timeout, interrupted if interrupt
         * @note it's useful when you want to use it in your own runloop, you can use zero timeout and call it in every frame
         */
        std::error_code poll(std::chrono::nanoseconds timeout);

        /**
         * Stop the poll
         * @note you can call this method in callback or other thread to interrupt the poll
         */
        void stop();

    private:
        reactor(const reactor&) = delete;
        reactor& operator=(const reactor&) = delete;

    private:
        short _count = 0;

#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // Unix, use kqueue
        int type(int filter, int flags);
        int alter(handle_t fd, int filter, int flags, int fflags, void *data);
        bool update();

        handle_t _kqueue = invalid_handle;

        std::set<timer*, timer::compare> _timers;
        std::unordered_set<basic_handle*> _cache;

#elif defined(__linux__)

        // Linux, use epoll
        int type(int events);

        handle_t _epoll = invalid_handle;

        chen::event _wakeup;
        std::unordered_set<basic_handle*> _cache;

#else

        // Windows, use WSAPoll
        int type(int events);
        bool update();

        chen::event _wakeup;
        chen::event _repoll;

        std::vector<struct ::pollfd> _events;
        std::set<timer*, timer::compare> _timers;
        std::unordered_map<handle_t, basic_handle*> _cache;

#endif
    };
}