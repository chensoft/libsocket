/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#if !defined(__linux__) && !defined(_WIN32)

#include <vector>

namespace chen
{
    /**
     * kqueue for FreeBSD, OS X(reactor model)
     * you should not use this class directly unless you want to implement your own event-based model
     * @attention in the following comments, LT means level-triggered, ET means edge-triggered
     */
    class kqueue
    {
    public:
        /**
         * Read(LT): event always occurs if the recv buffer has unread data
         * -----------------------------------------------------------------
         * Read(ET): event occurs only when new data arrives
         * if you read part of the data, event will not occur again unless new data arrives
         * -----------------------------------------------------------------
         * Write(LT): event always occurs if the send buffer is not full
         * -----------------------------------------------------------------
         * Write(ET): as long as the buffer is not full, the event always occurs after you call send()
         * this behavior is different than Linux's epoll
         * in epoll, the event occurs only when the state changes from "cannot output" to "can output"
         * -----------------------------------------------------------------
         * @attention since the socket has its own send buffer, you don't need to monitor the write event from the start
         * usually you should call send() first, if the method return EAGAIN then to wait for the write event occurs
         */
        static constexpr int OpcodeRead  = 1 << 0;
        static constexpr int OpcodeWrite = 1 << 1;

        /**
         * Once: event occurs only once
         * Edge: enable edge triggered, default is level triggered
         */
        static const int FlagOnce;
        static const int FlagEdge;

        /**
         * Read: read event occurs, you can read data from socket
         * -----------------------------------------------------------------
         * Write: you can write data to remote host
         * -----------------------------------------------------------------
         * End: socket disconnected or connection refused
         * -----------------------------------------------------------------
         * @attention you must monitor the read event if you want to know the end event
         * this behavior is different than Linux's epoll
         * in epoll, end event will always be monitored
         * -----------------------------------------------------------------
         * @attention you should read the rest of the data even if you received the end event
         * because server may send last message and then close the connection immediately
         * kqueue may report Read & End event or only report the End event
         */
        enum class Event {Read = 1, Write, End};

        typedef struct Data
        {
            Data() = default;
            Data(int fd, Event ev) : fd(fd), ev(ev) {}

            int   fd = -1;
            Event ev;
        } Data;

    public:
        kqueue();
        ~kqueue();

    public:
        /**
         * Set events for fd
         * @param opcode OpcodeRead, OpcodeWrite or combination of them
         * @param flag FlagOnce, FlagEdge or combination of them
         * -----------------------------------------------------------------
         * @attention although read & write events are separate in kqueue, but
         * epoll does not distinguish between them. since most of the servers
         * running Linux today, so I had to simulate the epoll's behaviour here.
         * Personally, I think kqueue's design is more flexible than epoll.
         */
        void set(int fd, int opcode, int flag = 0);

        /**
         * Delete all events for fd
         */
        void del(int fd);

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

    private:
        kqueue(const kqueue&) = delete;
        kqueue& operator=(const kqueue&) = delete;

    private:
        int  _fd = -1;     // kqueue handle
        bool _wk = false;  // is working
    };


    /**
     * reactor typedef
     */
    typedef kqueue reactor;
}

#endif