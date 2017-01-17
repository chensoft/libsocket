/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef __linux__

#include <socket/core/timer.hpp>

// -----------------------------------------------------------------------------
// timer
chen::timer::timer()
{
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
    return value >= this->_alarm;
}

void chen::timer::update(const std::chrono::high_resolution_clock::time_point &value)
{
    // reset alarm to next timestamp
    if (this->_value > std::chrono::nanoseconds::zero())
        this->_alarm = value + this->_value;
}

#endif