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
         * Invoke callback repeatedly after a period of time
         */
        void interval(const std::chrono::nanoseconds &value);

        /**
         * Invoke callback in a future calendar date
         */
        void future(const std::chrono::high_resolution_clock::time_point &value);

    public:
        /**
         * Timer info
         */
        bool repeat() const
        {
            return this->cycle() > std::chrono::nanoseconds::zero();
        }

        std::chrono::nanoseconds cycle() const
        {
            return this->_cycle;
        }

        std::chrono::high_resolution_clock::time_point alarm() const
        {
            return this->_alarm;
        }

    public:
        /**
         * Native timer handle
         * @note used by reactor only, fd is only valid under Linux
         */
        basic_handle& handle()
        {
            return this->_handle;
        }

        /**
         * Check if expired
         * @note used by reactor only, valid under non-Linux
         */
        bool expired(const std::chrono::high_resolution_clock::time_point &value) const;

        /**
         * Update timer
         * @note used by reactor only, valid under non-Linux, must erase timer from set before call it
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

    private:
        basic_handle _handle;  // use timerfd on Linux, calculate manually on other OS

        std::chrono::nanoseconds _cycle;  // time interval if timer is repeated, otherwise is zero
        std::chrono::high_resolution_clock::time_point _alarm;  // the next alarm time point
    };
}