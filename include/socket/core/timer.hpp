/**
 * Created by Jian Chen
 * @since  2017.01.07
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_handle.hpp>

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
         * Native timer event
         * you can use it in reactor, usually you register it with ModeRead, if
         * Readable event occurs then you can reset the timeout and do your jobs
         */
        basic_handle& handle() const;

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