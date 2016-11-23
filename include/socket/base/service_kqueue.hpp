/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#if !defined(__linux__) && !defined(_WIN32)

#include <socket/config.hpp>
#include <sys/event.h>
#include <vector>

namespace chen
{
    /**
     * kqueue for FreeBSD, OS X(reactor model)
     * you should not use this class directly unless you want to implement your own event-based model
     * @attention in the following comments, LT means level-triggered, ET means edge-triggered
     */
    class service_kqueue
    {
    public:
        /**
         * Read(LT): event always occurs if the recv buffer has unread data
         * ---------------------------------------------------------------------
         * Read(ET): event occurs only when new data arrives
         * if you read part of the data, event will not occur again unless new data arrives
         * ---------------------------------------------------------------------
         * Write(LT): event always occurs if the send buffer is not full
         * ---------------------------------------------------------------------
         * Write(ET): as long as the buffer is not full, the event always occurs after you call send()
         * this behavior is different than Linux's epoll
         * in epoll, the event occurs only when the state changes from "cannot output" to "can output"
         * ---------------------------------------------------------------------
         * @attention since the socket has its own send buffer, you don't need to monitor the write event from the start
         * usually you should call send() first, if the method return EAGAIN then to wait for the write event occurs
         */
        static constexpr int OpcodeRead  = 1 << 0;
        static constexpr int OpcodeWrite = 1 << 1;
        static constexpr int OpcodeRW    = OpcodeRead | OpcodeWrite;

        /**
         * Edge: enable edge triggered, default is level triggered
         * Once: event occurs only once
         */
        static constexpr int FlagEdge = EV_CLEAR;
        static constexpr int FlagOnce = EV_ONESHOT;

        /**
         * Readable: read event occurs, you can read data from socket
         * ---------------------------------------------------------------------
         * Writable: you can write data to remote host
         * ---------------------------------------------------------------------
         * Ended: socket disconnected or connection refused
         * ---------------------------------------------------------------------
         * @attention you must monitor the read event if you want to know the end event
         * this behavior is different than Linux's epoll
         * in epoll, the end event will always be monitored
         * ---------------------------------------------------------------------
         * @attention you should read the rest of the data even if you received the end event
         * because server may send last message and then close the connection immediately
         * kqueue may report Readable & Ended event or only report the Ended event
         */
        enum class Event {Readable = 1, Writable, Ended};

        /**
         * Only report custom data pointer and event type
         * user can pass an object's pointer when set fd
         * if event occurs then call object's callback
         */
        struct Data
        {
            Data() = default;
            Data(void *ptr, Event ev) : ptr(ptr), ev(ev) {}

            void *ptr;
            Event ev;
        };

    public:
        service_kqueue();
        ~service_kqueue();

    public:
        /**
         * Set events for fd, if Ended event occurs then fd will be removed
         * @param opcode OpcodeRead, OpcodeWrite or combination of them
         * @param flag FlagOnce, FlagEdge or combination of them
         * @param ptr user's custom data pointer
         * ---------------------------------------------------------------------
         * @attention although read & write events are separate in kqueue, but
         * epoll does not distinguish between them. since most of the servers
         * running Linux today, so I had to simulate the epoll's behaviour here.
         * Personally, I think kqueue's design is more flexible than epoll.
         */
        void set(handle_t fd, int opcode, int flag, void *ptr);

        /**
         * Delete all events for fd
         */
        void del(handle_t fd);

    public:
        /**
         * Poll events, with an optional timeout
         * when timeout is negative, it means wait forever, usually you can pass -1 to it
         * when timeout is zero, the poll method will return immediately, an event may or may not return
         * when timeout is positive, the time unit is second, e.g: 1.15 means 1.15 seconds to wait
         * @param cache pre allocated cache, if size < count then push result to it if needed
         * @param count how many events you want to monitor for
         * @return zero if user request to stop, timeout or interrupted
         */
        std::size_t poll(std::vector<Data> &cache, std::size_t count, double timeout = -1);

        /**
         * Poll events, return vector directly
         * @return empty if user request to stop, timeout or interrupted
         */
        std::vector<Data> poll(std::size_t count, double timeout = -1);

        /**
         * Stop the poll if kqueue is waiting for events
         */
        void stop();

    private:
        /**
         * Helper
         */
        Event event(int filter, int flags);
        int alter(handle_t fd, int filter, int flags, int fflags, void *ptr);

    private:
        service_kqueue(const service_kqueue&) = delete;
        service_kqueue& operator=(const service_kqueue&) = delete;

    private:
        handle_t _fd = invalid_handle;  // kqueue handle
    };
}

#endif