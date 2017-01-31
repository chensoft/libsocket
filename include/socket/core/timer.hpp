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
#ifndef __linux__
        /**
         * Check timer property
         */
        bool repeat() const
        {
            return this->_repeat;
        }

        std::chrono::nanoseconds cycle() const
        {
            return this->_cycle;
        }

        std::chrono::high_resolution_clock::time_point alarm() const
        {
            return this->_alarm;
        }

        /**
         * Calculate init value
         */
        void adjust(const std::chrono::high_resolution_clock::time_point &now);

        /**
         * Update timer value
         * @return true if timer expired after update, otherwise false
         */
        bool update(const std::chrono::high_resolution_clock::time_point &now);
#endif

    private:
        bool _repeat = false;
        basic_handle _handle;  // use timerfd on Linux, calculate manually on other OS

        std::chrono::nanoseconds _cycle;  // value when call timeout or interval
        std::chrono::high_resolution_clock::time_point _alarm;  // the next trigger time point
    };
}