/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef __linux__

#include <vector>

namespace chen
{
    namespace bsd
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
             * -----------------------------------------------------------------
             * Read(ET): event occurs only when new data arrives
             * if you read part of the data, event will not occur again unless new data arrives
             * -----------------------------------------------------------------
             * Write(LT): event always occurs if the send buffer is not full
             * -----------------------------------------------------------------
             * Write(ET): event occurs only when the state changes from "cannot output" to "can output"
             * this behavior is different than Unix's kqueue
             * in kqueue, as long as the buffer is not full, the event always occurs after you call send()
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
             * None: user request to exit from the poll or timeout
             * -----------------------------------------------------------------
             * Read: read event occurs, you can read data from socket
             * -----------------------------------------------------------------
             * Write: you can write data to remote host
             * -----------------------------------------------------------------
             * End: socket disconnected or connection refused
             * -----------------------------------------------------------------
             * @attention the end event is always be monitored automatically
             * this behavior is different than Unix's kqueue
             * if kqueue, you must monitor the read event, otherwise the end event will not be reported
             * -----------------------------------------------------------------
             * @attention you should read the rest of the data even if you received the end event
             * because server may send last message and then close the connection immediately
             * epoll may report Read & End event or only report the End event
             */
            enum class Event {None = 0, Read, Write, End};

            typedef struct Data
            {
                Data() = default;
                Data(int fd, Event ev) : fd(fd), ev(ev) {}

                int   fd = -1;
                Event ev = Event::None;
            } Data;

        public:
            epoll();
            ~epoll();

        public:
            /**
             * Set events for fd
             * @param opcode OpcodeRead, OpcodeWrite or combination of them
             * @param flag FlagOnce, FlagEdge or combination of them
             */
            void set(int fd, int opcode, int flag = 0);

            /**
             * Delete all events for fd
             */
            void del(int fd);

        public:
            /**
             * Poll a event, with an optional timeout
             * when timeout is negative, it means wait forever, usually you can pass -1 to it
             * when timeout is zero, the poll method will return immediately, an event may or may not return
             * when timeout is positive, the time unit is second, e.g: 1.15 means 1.15 seconds to wait
             * @return Data.ev is None if user request to exit the poll or timeout
             */
            Data poll(double timeout = - 1);

            /**
             * Fetch multiple events, with an optional timeout
             * @param count how many events you want to monitor for
             * @param timeout the same as the poll method's timeout param
             * @return empty vector if user request to exit the poll or timeout
             */
            std::vector<Data> fetch(int count, double timeout = -1);

            /**
             * Exit the poll if epoll is waiting for events
             */
            void exit();

        private:
            /**
             * Helper
             */
            Event event(unsigned events);

        private:
            epoll(const epoll&) = delete;
            epoll& operator=(const epoll&) = delete;

        private:
            int _fd = -1;  // epoll handle
            int _ef = -1;  // eventfd handle
        };


        /**
         * reactor typedef
         */
        typedef epoll reactor;
    }
}

#endif