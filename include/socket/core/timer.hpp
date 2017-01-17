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
         * Invoke callback only once in a future calendar date
         */
        void future(const std::chrono::high_resolution_clock::time_point &value);

    public:
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
         * @note used by reactor under non-Linux only
         */
        bool expired(const std::chrono::high_resolution_clock::time_point &value) const;

        /**
         * Update timer's alarm value
         * @note used by reactor only
         */
        void update(const std::chrono::high_resolution_clock::time_point &value);

    public:
        /**
         * Comparator class
         * @note used by reactor only
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

        bool _repeat = false;

        std::chrono::nanoseconds _cycle;  // value when call timeout or interval
        std::chrono::nanoseconds _alarm;  // the next trigger unix timestamp val
    };
}