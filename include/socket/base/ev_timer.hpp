/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/ev_base.hpp>
#include <functional>
#include <chrono>

namespace chen
{
    class ev_timer: public ev_base
    {
    public:
        ev_timer(std::function<void ()> cb = nullptr);

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

    public:
        /**
         * Attach callback
         */
        void attach(std::function<void ()> cb);

    public:
        /**
         * Timer properties
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
        void setup(const std::chrono::high_resolution_clock::time_point &now);

        /**
         * Check if timer expire
         */
        bool expire(const std::chrono::high_resolution_clock::time_point &now) const
        {
            return now >= this->_alarm;
        }

        /**
         * Update timer value
         */
        void update(const std::chrono::high_resolution_clock::time_point &now)
        {
            if (this->_repeat)
                this->_alarm = now + this->_cycle;
        }

        /**
         * Comparator, used in conjunction with multiset
         * @note if the cycle value changes, you should re-add the timer to multiset
         */
        struct Compare
        {
            bool operator()(const ev_timer *a, const ev_timer *b) const
            {
                return a->cycle() < b->cycle();
            }
        };

    protected:
        /**
         * At least one event has occurred
         */
        virtual void onEvent(int type);

    private:
        bool _repeat = false;
        std::function<void ()> _notify;

        std::chrono::nanoseconds _cycle;  // value when call timeout or interval
        std::chrono::high_resolution_clock::time_point _alarm;  // the next trigger time point
    };
}