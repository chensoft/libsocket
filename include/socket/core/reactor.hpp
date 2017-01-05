/**
 * Created by Jian Chen
 * @since  2016.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/core/event.hpp>
#include <unordered_map>
#include <system_error>
#include <functional>
#include <vector>
#include <mutex>

namespace chen
{
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
         * @note If you use edge-triggered and multiple threads are calling poll
         * method, reactor does not guarantee that only one thread get the
         * events, only FlagOnce(except on Windows) guaranteed this feature
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
         * event, server may send last message and then close the connection immediately.
         * the backend may report readable & closed event or only report the closed event
         */
        typedef int Type;

        static const Type Readable;
        static const Type Writable;
        static const Type Closed;

        /**
         * Event callback
         * @note use bitwise and to check the event type, e.g: if (type & Readable)
         * @note if you want to bind custom params to callback, you can use std::bind
         */
        typedef std::function<void (Type type)> callback;

    public:
        reactor(int count = 64);  // events number used in backend, ignored on Windows
        ~reactor();

    public:
        /**
         * Monitor event
         * @param mode ModeRead, ModeWrite and etc
         * @param flag FlagOnce, FlagEdge and etc
         */
        void set(handle_t fd, callback cb, int mode, int flag);

        /**
         * Delete event
         * @note don't forget to delete it when it is destroyed
         */
        void del(handle_t fd);

    public:
        /**
         * Run loop forever, unless user request to stop
         */
        void run();

        /**
         * Poll events and notify
         * @param timeout unit is second(e.g: 1.15 means 1.15 seconds), forever if negative, return immediately if zero
         * @return empty if an event is handled, operation_canceled if stop, timed_out if timeout, interrupted if interrupt
         * @note this method is useful when you have your own runloop, you can use zero timeout and call it in every frame
         */
        std::error_code poll(double timeout = -1);

        /**
         * Stop the loop
         * @note you can call this method in callback or other thread to interrupt the loop
         */
        void stop();

    private:
        reactor(const reactor&) = delete;
        reactor& operator=(const reactor&) = delete;

    private:
#if !defined(__linux__) && !defined(_WIN32)

        // kqueue
        Type type(int filter, int flags);
        int alter(handle_t fd, int filter, int flags, int fflags, void *data);

        handle_t _kqueue = invalid_handle;

#elif defined(__linux__)

        // epoll
        Type type(int events);

        chen::event _wakeup;

        handle_t _epoll = invalid_handle;

#else

        // WSAPoll
        Type type(int events);

        chen::event _wakeup;

        std::vector<struct ::pollfd> _cache;
        std::unordered_map<handle_t, int> _flags;

#endif

        const int _count = 0;

        std::mutex _mutex;
        std::unordered_map<handle_t, callback> _store;
    };
}