/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef _WIN32

#include <vector>

namespace chen
{
    namespace bsd
    {
        /**
         * todo use WSAEventSelect & WaitForSingleObject & WSAEnumEvents ?
         * WSAPoll for Windows(reactor model)
         * Windows's most efficient model is IOCP, but IOCP is a proactor model, so we use WSAPoll here
         * you should not use this class directly unless you want to implement your own event-based model
         * @attention WSAPoll does not provide edge triggering
         */
        class poller
        {
        public:
            /**
             * Read: event always occurs if the recv buffer has unread data
             * -----------------------------------------------------------------
             * Write: event always occurs if the send buffer is not full
             * -----------------------------------------------------------------
             * @attention since the socket has its own send buffer, you don't need to monitor the write event from the start
             * usually you should call send() first, if the method return EAGAIN then to wait for the write event occurs
             */
            static constexpr int OpcodeRead  = 1 << 0;
            static constexpr int OpcodeWrite = 1 << 1;

            /**
             * Once: event occurs only once
             * Edge: JUST A PLACEHOLDER HERE, NO EFFECT
             * @attention since WSAPoll doesn't support edge-triggered, FlagEdge is just a placeholder here
             */
            static const int FlagOnce;
            static const int FlagEdge;  // just a placeholder, in order to keep ABI compatible with kqueue & epoll

            /**
             * None: user request to stop from the poll or timeout
             * -----------------------------------------------------------------
             * Read: read event occurs, you can read data from socket
             * -----------------------------------------------------------------
             * Write: you can write data to remote host
             * -----------------------------------------------------------------
             * End: socket disconnected or connection refused
             * -----------------------------------------------------------------
             * todo verify
             * @attention you must monitor the read event if you want to know the end event
             * this behavior is different than Linux's epoll
             * in epoll, end event will always be monitored
             * -----------------------------------------------------------------
             * todo verify
             * @attention you should read the rest of the data even if you received the end event
             * because server may send last message and then close the connection immediately
             * kqueue may report Read & End event or only report the End event
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
            poller();
            ~poller();

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
             * @return Data.ev is None if user request to stop the poll or timeout
             */
            Data poll(double timeout = - 1);

            /**
             * Fetch multiple events, with an optional timeout
             * @param count how many events you want to monitor for
             * @param timeout the same as the poll method's timeout param
             * @return empty vector if user request to stop the poll or timeout
             */
            std::vector<Data> fetch(int count, double timeout = -1);

            /**
             * Stop the poll if poller is waiting for events
             */
            void stop();

        private:
            /**
             * Helper
             */
            Event event(unsigned events);

        private:
            poller(const poller&) = delete;
            poller& operator=(const poller&) = delete;

        private:
            int _fd = -1;  // epoll handle
            int _ef = -1;  // todo eventfd handle
        };


        /**
         * todo
         * reactor typedef
         */
        typedef poller reactor;
    }
}

#endif