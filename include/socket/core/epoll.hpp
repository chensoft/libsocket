/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef __linux__

#include <socket/config.hpp>
#include <sys/epoll.h>
#include <vector>

// Android support these flags but ndk didn't define them
// unless you compile with android-21 or higher api level
// so we define these macros here to maintain consistency
#ifndef EPOLLONESHOT
#define EPOLLONESHOT 0x40000000
#endif

#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x00002000
#endif

namespace chen
{
    /**
     * epoll for Linux(reactor model)
     * you should not use this class directly unless you want to implement your own event-based model
     * @attention in the following comments, LT means level-triggered, ET means edge-triggered
     */
    class epoll
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
         * Write(ET): event occurs only when the state changes from "cannot output" to "can output"
         * this behavior is different than Unix's kqueue
         * in kqueue, as long as the buffer is not full, the event always occurs after you call send()
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
        static constexpr int FlagEdge = EPOLLET;
        static constexpr int FlagOnce = EPOLLONESHOT;
        
        /**
         * Readable: read event occurs, you can read data from socket
         * ---------------------------------------------------------------------
         * Writable: you can write data to remote host
         * ---------------------------------------------------------------------
         * Ended: socket disconnected or connection refused
         * ---------------------------------------------------------------------
         * @attention the end event is always be monitored
         * this behavior is different than Unix's kqueue
         * in kqueue, you must monitor the read event, otherwise the end event will not be reported
         * ---------------------------------------------------------------------
         * @attention you should read the rest of the data even if you received the end event
         * because server may send last message and then close the connection immediately
         * epoll may report Readable & Ended event or only report the Ended event
         */
        enum class Event {Readable = 1, Writable, Ended};
        
        typedef struct Data
        {
            Data() = default;
            Data(handle_t fd, Event ev) : fd(fd), ev(ev) {}
            
            handle_t fd = invalid_handle;
            Event ev;
        } Data;
        
    public:
        epoll();
        ~epoll();
        
    public:
        /**
         * Set events for fd, if Ended event occurs then fd will be removed
         * @param opcode OpcodeRead, OpcodeWrite or combination of them
         * @param flag FlagOnce, FlagEdge or combination of them
         */
        void set(handle_t fd, int opcode, int flag = 0);
        
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
         * @param count how many events you want to monitor, just a hint, final events may greater than this
         * @return the final events count, or zero if user request to stop, timeout or interrupted
         * @attention the number of events may greater than count because we treat read
         * and write as separate events, but epoll may report them as a single event
         */
        std::size_t poll(std::vector<Data> &cache, std::size_t count, double timeout = -1);
        
        /**
         * Poll events, return vector directly
         * @return empty if user request to stop or timeout
         */
        std::vector<Data> poll(std::size_t count, double timeout = -1);
        
        /**
         * Stop the poll if epoll is waiting for events
         */
        void stop();

    private:
        epoll(const epoll&) = delete;
        epoll& operator=(const epoll&) = delete;
        
    private:
        handle_t _fd = invalid_handle;  // epoll handle
        handle_t _ef = invalid_handle;  // eventfd handle
    };
}

#endif