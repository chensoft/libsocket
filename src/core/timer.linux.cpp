/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/timer.hpp>
#include <chen/sys/sys.hpp>
#include <sys/timerfd.h>

// -----------------------------------------------------------------------------
// timer
chen::timer::timer()
{
    auto fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    if (fd < 0)
        throw std::system_error(sys::error(), "timer: failed to create timerfd");

    this->_handle.change(fd);
}

chen::timer::~timer()
{
}

void chen::timer::timeout(const std::chrono::nanoseconds &value)
{
    this->_repeat = false;
    this->_value  = value;
    this->_alarm  = std::chrono::high_resolution_clock::time_point();
}

void chen::timer::interval(const std::chrono::nanoseconds &value)
{
    this->_repeat = true;
    this->_value  = value;
    this->_alarm  = std::chrono::high_resolution_clock::time_point();
}

void chen::timer::future(const std::chrono::high_resolution_clock::time_point &value)
{
    this->_repeat = false;
    this->_value  = std::chrono::nanoseconds::zero();
    this->_alarm  = value;
}

bool chen::timer::expired(const std::chrono::high_resolution_clock::time_point &value) const
{
    // unused under Linux
    return value >= this->_alarm;
}

void chen::timer::update(const std::chrono::high_resolution_clock::time_point &value)
{
    auto zero = std::chrono::nanoseconds::zero();
    auto time = ::itimerspec{};

    if (this->_value > zero)
    {
        // timeout or interval
        auto cycle = this->_value.count();

        time.it_value.tv_sec  = cycle / 1000000000;
        time.it_value.tv_nsec = cycle % 1000000000;

        if (this->_repeat)
            time.it_interval = time.it_value;
    }
    else if (this->_value == zero)
    {
        // future date
        auto alarm = (this->_alarm.time_since_epoch() - value.time_since_epoch()).count();

        if (alarm > 0)
        {
            time.it_value.tv_sec  = alarm / 1000000000;
            time.it_value.tv_nsec = alarm % 1000000000;
        }
        else
        {
            time.it_value.tv_nsec = 1;  // just trigger it
        }
    }

    // use relative time value
    if (::timerfd_settime(this->_handle, 0, &time, nullptr) < 0)
        throw std::system_error(sys::error(), "timer: failed to update time");
}

#endif