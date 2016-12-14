/**
 * Created by Jian Chen
 * @since  2016.12.13
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_socket.hpp>
#include <unordered_map>
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
         * Read(ET): event occurs only when new data arrives if you read part
         * of the data, event will not occur again unless new data arrives
         * ---------------------------------------------------------------------
         * Write(LT): event always occurs if the send buffer is not full
         * ---------------------------------------------------------------------
         * Write(ET, kqueue): as long as the buffer is not full, the event
         * always occurs after you call send()
         * Write(ET, epoll): event occurs only when the state changes from
         * "cannot output" to "can output"
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
         * Readable: read event occurs, you can read data from socket
         * ---------------------------------------------------------------------
         * Writable: you can write data to remote host
         * ---------------------------------------------------------------------
         * Closed: socket disconnected or connection refused
         * ---------------------------------------------------------------------
         * @note in epoll, closed event is always be monitored, in kqueue and poll
         * you must monitor the read event if you want to receive the closed event
         * ---------------------------------------------------------------------
         * @note you should read the rest of the data even if you received the closed
         * event, server may send last message and then close the connection immediately
         * the backend may report readable & closed event or only report the closed event
         */
        enum class Type {Readable = 1, Writable, Closed};

        /**
         * Event data, only report custom data pointer and event type, user can pass an
         * object's pointer when set fd, if event occurs then call the object's callback
         */
        struct Data
        {
            Data() = default;
            Data(void *data, Type type) : data(data), type(type) {}

            void *data;
            Type  type;
        };

    public:
        reactor();
        ~reactor();

    public:
        /**
         * Set events for fd
         * @param ptr user's custom data pointer
         * @param mode ModeRead, ModeWrite or combination of them
         * @param flag FlagOnce, FlagEdge or combination of them
         * ---------------------------------------------------------------------
         * @note although read & write events are separate in kqueue, but epoll
         * does not distinguish between them. since most of the servers running
         * Linux today, so I had to simulate the epoll's behaviour here.
         * Personally, I think kqueue's design is more flexible than epoll
         */
        void set(handle_t fd, void *data, int opcode, int flag);

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
         * @return the final events count, or zero if user request to stop, timeout, interrupted or no fds to monitor
         * @note the number of events may greater than count because we treat the read
         * and write as separate events, but backend may report them as a single event
         */
        std::size_t poll(std::vector<Data> &cache, std::size_t count, double timeout = -1);

        /**
         * Poll events, return vector directly
         * @return empty if user request to stop, timeout or interrupted
         */
        std::vector<Data> poll(std::size_t count, double timeout = -1);

        /**
         * Stop the poll if poller is waiting for events
         */
        void stop();

    private:
        reactor(const reactor&) = delete;
        reactor& operator=(const reactor&) = delete;

    private:
#if !defined(__linux__) && !defined(_WIN32)
        Type event(int filter, int flags);
        int alter(handle_t fd, int filter, int flags, int fflags, void *data);

        handle_t _fd = invalid_handle;  // kqueue handle
#elif defined(__linux__)
        handle_t _fd = invalid_handle;  // epoll handle
        handle_t _ef = invalid_handle;  // eventfd handle
#else
        struct Detail
        {
            Detail() = default;
            Detail(int flag, void *data) : flag(flag), data(data) {}

            int   flag = 0;
            void *data = nullptr;
        };

        basic_socket _wake;

        std::vector<::pollfd> _fds;
        std::unordered_map<handle_t, Detail> _map;
#endif
    };
}

//namespace chen
//{
//    // -------------------------------------------------------------------------
//    // reactor_delegate
//    class reactor_delegate
//    {
//    public:
//        virtual ~reactor_delegate() = default;
//
//    public:
//        virtual void onReadable() {}
//        virtual void onWritable() {}
//        virtual void onEnded() {}
//    };
//
//
//    // -------------------------------------------------------------------------
//    // reactor
//    class reactor
//    {
//    public:
//        /**
//         * Set events and callback
//         */
//        void set(handle_t fd, reactor_delegate *delegate, int opcode, int flag);
//
//    public:
//        /**
//         * Run the loop
//         * @param count how many events you want to monitor per loop, it's just a hint, default is 1
//         * @note this method will not return unless user request to stop, timeout or interrupted
//         */
//        void run();
//        void run(std::size_t count);
//
//        /**
//         * Stop the loop
//         * @note you can call this method in callback or other thread
//         */
//        void stop();
//    };
//}