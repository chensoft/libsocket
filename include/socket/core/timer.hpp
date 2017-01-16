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
         * Invoke callback from now on after a period of time
         */
        void future(const std::chrono::nanoseconds &value);

        /**
         * Invoke callback in a future calendar date
         */
        void future(const std::chrono::system_clock::time_point &value);

    public:
        /**
         * Timer info
         */
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
         * @note used by reactor only, fd is only valid under Linux
         */
        basic_handle& handle()
        {
            return this->_handle;
        }

        /**
         * Check if expire
         * @note used by reactor only, valid under non-Linux
         */
        bool expire(const std::chrono::system_clock::time_point &value) const;

        /**
         * Update timer
         * @note used by reactor only, valid under non-Linux, must erase timer from set before call it
         */
        void update(const std::chrono::system_clock::time_point &value);

        /**
         * Comparator class
         */
        struct compare
        {
            bool operator()(const timer *a, const timer *b) const
            {
                return a->_target < b->_target;
            }
        };

    private:
        basic_handle _handle;  // use timerfd on Linux, calculate manually on other OS

        bool _repeat = false;

        std::chrono::nanoseconds _origin;  // origin timestamp or interval
        std::chrono::nanoseconds _target;  // the next trigger timestamp
    };
}