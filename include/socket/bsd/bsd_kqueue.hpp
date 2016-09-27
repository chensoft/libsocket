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
            enum class Filter {Read = 1, Write};
            enum class Event  {None = 0, Read, Write, End};  // End means disconnected or connection refused

            static constexpr std::uint16_t FlagOnce = 1 << 0;  // one shot
            static constexpr std::uint16_t FlagEdge = 1 << 1;  // edge trigger

            typedef struct
            {
                int   fd = -1;
                Event ev = Event::None;
                void *dt = nullptr;
            } Data;

        public:
            kqueue();
            ~kqueue();

        public:
            /**
             * Add specific event for fd
             * @param flag FlagOnce, FlagEdge or combination of them
             * @param data user custom data
             */
            void add(int fd, Filter filter, std::uint16_t flag = 0, void *data = nullptr);

            /**
             * Delete all events or specific event
             */
            void del(int fd);
            void del(int fd, Filter filter);

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
            std::int16_t filter(Filter filter);
            Event event(std::int16_t filter, std::uint16_t flags);

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