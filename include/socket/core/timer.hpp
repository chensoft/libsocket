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
    class timer
    {
    public:
        timer();
        ~timer();

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
        timer(const timer&) = delete;
        timer& operator=(const timer&) = delete;

    private:
#if !defined(__linux__) && !defined(_WIN32)

        // unix

#elif defined(__linux__)

        // linux

#else

        // win

#endif
    };
}