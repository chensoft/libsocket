/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#ifdef __linux__

#include <cstdint>

namespace chen
{
    namespace bsd
    {
        /**
         * epoll for Linux
         * you should not use this class directly unless you want to implement your own event-based model
         */
        class epoll
        {
        public:
            enum class Event {None = 0, Read, Write, End};  // End means disconnected or connection refused

            /**
             * Under the edge trigger mode, the write event will occur ONLY when the state changes from "cannot output" to "can output"
             * if you send a small chunk of data and wait for next write event, epoll will not notify you
             * usually you should wait for write event only when the send method return EAGAIN error
             * @notice this behavior is different with kqueue
             */
            enum class Opcode {Read = 1, Write};

            static constexpr std::uint16_t FlagOnce = 1 << 0;  // one shot
            static constexpr std::uint16_t FlagEdge = 1 << 1;  // edge trigger

            typedef struct
            {
                int   fd = -1;
                Event ev = Event::None;
            } Data;

        public:
            epoll();
            ~epoll();

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
            std::uint32_t opcode(Opcode opcode);
            Event event(std::uint32_t events);

        private:
            epoll(const epoll&) = delete;
            epoll& operator=(const epoll&) = delete;

        private:
            int _fd = -1;  // epoll handle
            int _pp[2]{};  // use pipe to exit the poll operation, 0 is read fd, 1 is write fd
        };
    }
}

#endif