/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/basic_handle.hpp>
#include <chrono>

namespace chen
{
    class timer
    {
    public:
        timer();
        ~timer();

    public:
        /**
         * Invoke callback only once after a period of time
         * @note timer is a steady clock, will not affected by the wall clock
         */
        void timeout(const std::chrono::nanoseconds &value);

        /**
         * Invoke callback repeatedly after a period of time
         * @note timer is a steady clock, will not affected by the wall clock
         */
        void interval(const std::chrono::nanoseconds &value);

        /**
         * Invoke callback from now on after a period of time
         * @note timer is a system clock, will affected by the wall clock
         */
        void future(const std::chrono::nanoseconds &value);

        /**
         * Invoke callback in a future calendar date
         * @note timer is a system clock, will affected by the wall clock
         */
        void future(const std::chrono::time_point<std::chrono::system_clock> &value);

    public:
        /**
         * Timer info
         */
        bool steady() const
        {
            return this->_steady;
        }

        bool repeat() const
        {
            return this->_repeat;
        }

        std::chrono::nanoseconds origin() const
        {
            return this->_origin;
        }

        std::chrono::nanoseconds target() const
        {
            return this->_target;
        }

    public:
        /**
         * Native timer handle
         * @note used by reactor only, valid under the Linux
         */
        basic_handle& handle();

        /**
         * Update the timer
         * @note used by reactor only, valid under non-Linux
         */
        void update(const std::chrono::time_point<std::chrono::steady_clock> &value);
        void update(const std::chrono::time_point<std::chrono::system_clock> &value);

    private:
        bool _steady = false;
        bool _repeat = false;

        std::chrono::nanoseconds _origin;  // origin time set by timeout, interval or future
        std::chrono::nanoseconds _target;  // the next trigger time, it's a unix timestamp

#ifdef __linux__

        // Linux, use timerfd

#else

        // Unix, Windows, calculate the time manually

#endif
    };
}