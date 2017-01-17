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
         */
        void timeout(const std::chrono::nanoseconds &value);

        /**
         * Invoke callback only once in a future calendar date
         */
        void future(const std::chrono::high_resolution_clock::time_point &value);

        /**
         * Invoke callback repeatedly after a period of time
         */
        void interval(const std::chrono::nanoseconds &value);

        /**
         * Native timer handle
         * @note fd is only valid under Linux
         */
        basic_handle& handle()
        {
            return this->_handle;
        }

    public:
#ifdef __linux__

        /**
         * Reset timer state
         */
        void reset();

        /**
         * Read data from fd
         */
        void clear();

#else

        /**
         * Timer property
         */
        bool repeat() const
        {
            return this->_repeat;
        }

        std::chrono::nanoseconds cycle() const
        {
            return this->_cycle;
        }

        std::chrono::nanoseconds alarm() const
        {
            return this->_alarm;
        }

        /**
         * Check if expired
         */
        bool expired(const std::chrono::high_resolution_clock::time_point &value) const;

        /**
         * Update timer's alarm value
         */
        void update(const std::chrono::high_resolution_clock::time_point &value);

        /**
         * Comparator class
         */
        struct compare
        {
            bool operator()(const timer *a, const timer *b) const
            {
                return a->_alarm < b->_alarm;
            }
        };

#endif

    private:
        basic_handle _handle;  // use timerfd on Linux, calculate manually on other OS

        bool _repeat = false;

        std::chrono::nanoseconds _cycle;  // value when call timeout or interval
        std::chrono::nanoseconds _alarm;  // the next trigger unix timestamp val
    };
}