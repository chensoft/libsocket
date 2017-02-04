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
        ev_timer();
        ev_timer(std::function<void ()> cb);
        ~ev_timer();

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

        /**
         * Native handle value
         */
        virtual handle_t native() const
        {
            return invalid_handle;  // no use in timer
        }

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
        void adjust(const std::chrono::high_resolution_clock::time_point &now);

        /**
         * Update timer value
         * @return true if timer expired after update, otherwise false
         */
        bool update(const std::chrono::high_resolution_clock::time_point &now);

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