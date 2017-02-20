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
        enum class Flag {Normal, Future, Repeat};

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
        void future(const std::chrono::nanoseconds &value);
        void future(const std::chrono::steady_clock::time_point &value);

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
        Flag flag() const
        {
            return this->_flag;
        }

        std::chrono::nanoseconds time() const
        {
            return this->_time;
        }

        std::chrono::steady_clock::time_point when() const
        {
            return this->_when;
        }

        /**
         * Calculate init value
         */
        void setup(const std::chrono::steady_clock::time_point &now);

        /**
         * Check if timer expire
         */
        bool expire(const std::chrono::steady_clock::time_point &now) const
        {
            return now >= this->_when;
        }

        /**
         * Update timer value
         */
        void update(const std::chrono::steady_clock::time_point &now)
        {
            if (this->_flag == Flag::Repeat)
                this->_when = now + this->_time;
        }

        /**
         * Comparator, used in conjunction with multiset
         * @note if the cycle value changes, you should re-add the timer to multiset
         */
        struct Compare
        {
            bool operator()(const ev_timer *a, const ev_timer *b) const
            {
                return a->time() < b->time();
            }
        };

    protected:
        /**
         * At least one event has occurred
         */
        virtual void onEvent(int type);

    private:
        Flag _flag = Flag::Normal;

        std::chrono::nanoseconds _time;  // the interval between two trigger points
        std::chrono::steady_clock::time_point _when;  // the next trigger point

        std::function<void ()> _notify;
    };
}