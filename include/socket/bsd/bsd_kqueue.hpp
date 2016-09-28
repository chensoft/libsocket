/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#if !defined(__linux__) && !defined(_WIN32)

#include <cstdint>

namespace chen
{
    namespace bsd
    {
        /**
         * kqueue for FreeBSD, OS X
         * you should not use this class directly unless you want to implement your own event-based model
         */
        class kqueue
        {
        public:
            /**
             * Under the edge trigger mode, the write event will ALWAYS occur if the send buffer is not full filled
             * usually you should wait for write event only when the send method return EAGAIN error
             * @notice this behavior is different with epoll
             */
            enum class Opcode {Read = 1, Write};

            /**
             * :-) None  means user request exit from poll
             * :-) Read  means you can read data from socket
             * :-) Write means you can write data to remote
             * :-) End   means disconnected or connection refused
             * Notice:
             * :-) If you don't listen Read event, then the End event will not reported by kqueue
             *     this behavior is different with epoll
             * :-) Even if you receive the End event, you can still read data from socket until an error occurs
             *     because server may send last message and then close the connection immediately
             *     kqueue may report Read & End event or only report the End event
             */
            enum class Event {None = 0, Read, Write, End};  // End means disconnected or connection refused

            static constexpr std::uint16_t FlagOnce = 1 << 0;  // one shot
            static constexpr std::uint16_t FlagEdge = 1 << 1;  // edge trigger

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
             * Add specific event for fd
             * @param flag FlagOnce, FlagEdge or combination of them
             */
            void add(int fd, Opcode opcode, std::uint16_t flag = 0);

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
             * Conversion
             */
            std::int16_t opcode(Opcode opcode);
            Event event(std::int16_t opcode, std::uint16_t flags);

        private:
            kqueue(const kqueue&) = delete;
            kqueue& operator=(const kqueue&) = delete;

        private:
            int _fd = -1;  // kqueue handle
            int _pp[2]{};  // use pipe to exit the poll operation, 0 is read fd, 1 is write fd
        };
    }
}

#endif