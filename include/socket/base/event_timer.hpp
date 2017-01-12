/**
 * Created by Jian Chen
 * @since  2017.01.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/config.hpp>

namespace chen
{
    class event_timer
    {
    public:
        event_timer();
        ~event_timer();

    public:
        // todo allow specify date and chrono
        // todo allow specify only once or repeated
        /**
         * Set timeout
         */
        void set(double timeout);

        /**
         * Clear timeout
         */
        void reset();

        // todo allow retrieve original time
        // todo allow retrieve interval time

        /**
         * Native event handle
         * you can use it in reactor, usually you register it with ModeRead, if
         * Readable event occurs then you can reset the timeout and do your jobs
         */
        handle_t native() const;

    private:
        event_timer(const event_timer&) = delete;
        event_timer& operator=(const event_timer&) = delete;

    private:
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

        // unix

#elif defined(__linux__)

        // linux

#else

        // win

#endif
    };
}