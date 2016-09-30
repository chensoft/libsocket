/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#if !defined(__linux__) && !defined(_WIN32)

namespace chen
{
    namespace bsd
    {
        /**
         * kqueue for FreeBSD, OS X
         * you should not use this class directly unless you want to implement your own event-based model
         * @notice in the following comments, LT means level-triggered, ET means edge-triggered
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
             * Write(ET): event occurs when the buffer is not full and then you call the send() method
             * this behavior is different than Linux's epoll
             * in epoll, the event occurs only when the state changes from "cannot output" to "can output"
             * in kqueue, as long as the buffer is not full, the event always occurs after you call send()
             * -----------------------------------------------------------------
             * @notice since the socket has its own send buffer, you don't need to monitor the write event from the start
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
             * None: user request to exit from the poll
             * -----------------------------------------------------------------
             * Read: read event occurs, you can read data from socket
             * -----------------------------------------------------------------
             * Write: you can write data to remote host
             * -----------------------------------------------------------------
             * End: socket disconnected or connection refused
             * -----------------------------------------------------------------
             * @notice you must monitor the read event if you want to know the end event
             * this behavior is different than Linux's epoll
             * in epoll, end event will always be monitored
             * if kqueue, you must monitor the read event, otherwise the end event will not be reported
             * -----------------------------------------------------------------
             * @notice you should read the rest of the data even if you received the end event
             * because server may send last message and then close the connection immediately
             * kqueue may report Read & End event or only report the End event
             */
            enum class Event {None = 0, Read, Write, End};

            typedef struct
            {
                int   fd = -1;
                Event ev = Event::None;
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
             * @notice although read & write events are separate in kqueue, but
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
             * Poll a event
             * @result Data.ev is None if user request to exit the poll
             */
            Data poll();

            /**
             * Exit the poll
             */
            void exit();

        private:
            /**
             * Helper
             */
            Event event(int opcode, int flags);

        private:
            kqueue(const kqueue&) = delete;
            kqueue& operator=(const kqueue&) = delete;

        private:
            int _fd = -1;  // kqueue handle
            int _pp[2]{};  // use pipe to exit the poll, 0 is read fd, 1 is write fd
        };
    }
}

#endif