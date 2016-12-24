/**
 * Created by Jian Chen
 * @since  2016.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_socket.hpp>
#include <unordered_map>
#include <functional>
#include <vector>

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
         * send buffer is not full
         * ---------------------------------------------------------------------
         * @note since the socket has its own send buffer, you don't need to monitor
         * the write event from the start, usually you should call send() first, if
         * the method return EAGAIN then to wait for the write event occurs
         */
        static constexpr int ModeRead  = 1 << 0;
        static constexpr int ModeWrite = 1 << 1;
        static constexpr int ModeRW    = ModeRead | ModeWrite;

        /**
         * Event flag
         * @note Windows does not support edge-triggered
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
        enum class Type {Readable = 1, Writable, Closed};

        /**
         * Event callback
         * @note if you want to bind custom params to callback, you can use std::bind
         */
        typedef std::function<void (chen::handle_t fd, chen::reactor::Type type)> callback;

    public:
        reactor(std::size_t events = 64);  // events number used in backend
        ~reactor();

    public:
        /**
         * Monitor events for fd
         * @param mode ModeRead, ModeWrite and etc
         * @param flag FlagOnce, FlagEdge and etc
         * ---------------------------------------------------------------------
         * @note although read & write events are separate in kqueue, but epoll
         * does not distinguish between them. since most of the servers running
         * Linux today, so I had to simulate the epoll's behaviour here.
         * Personally, I think kqueue's design is more flexible than epoll
         */
        void set(handle_t fd, callback cb, int mode, int flag);

        /**
         * Delete all events for fd
         * @note don't forget to delete fd when it is destroyed
         */
        void del(handle_t fd);

    public:
        /**
         * Run loop forever, unless user request to stop, timeout or interrupted
         * @param timeout unit is second(e.g: 1.15 means 1.15 seconds), forever if negative, return immediately if zero
         */
        void run(double timeout = -1);

        /**
         * Run loop only once
         * @param timeout unit is second(e.g: 1.15 means 1.15 seconds), forever if negative, return immediately if zero
         * @return true if an event is handled, false if stop, timeout or interrupted
         * @note this method is useful when you have your own runloop, you can use zero timeout and call it in every frame
         */
        bool once(double timeout = -1);

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
        Type event(int filter, int flags);
        int alter(handle_t fd, int filter, int flags, int fflags, void *data);

        handle_t _kqueue = invalid_handle;

        std::vector<struct ::kevent> _events;
        std::unordered_map<handle_t, callback> _callbacks;

#elif defined(__linux__)

        // epoll
        handle_t _epoll = invalid_handle;
        handle_t _wake  = invalid_handle;  // eventfd handle

        std::vector<struct ::epoll_event> _events;
        std::unordered_map<handle_t, callback> _callbacks;

#else

        // WSAPoll

#endif
    };
}