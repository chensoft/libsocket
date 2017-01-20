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
    this->_cycle = value;
    this->_alarm = std::chrono::high_resolution_clock::time_point::min();
}

void chen::timer::future(const std::chrono::high_resolution_clock::time_point &value)
{
    this->_cycle = std::chrono::nanoseconds::zero();
    this->_alarm = value;
}

void chen::timer::interval(const std::chrono::nanoseconds &value)
{
    this->_cycle = value;
    this->_alarm = std::chrono::high_resolution_clock::time_point();
}

bool chen::timer::repeat() const
{
    return (this->_cycle != std::chrono::nanoseconds::zero()) && (this->_alarm == std::chrono::high_resolution_clock::time_point::min());
}

std::chrono::nanoseconds chen::timer::cycle() const
{
    return this->_cycle;
}

std::chrono::high_resolution_clock::time_point chen::timer::alarm() const
{
    return this->_alarm;
}

bool chen::timer::update()
{
    if (this->_alarm == std::chrono::high_resolution_clock::time_point::min())
    {
        this->_alarm = std::chrono::high_resolution_clock::now() + this->_cycle;
        this->_cycle = std::chrono::nanoseconds::zero();
    }
    else if (this->_cycle > std::chrono::nanoseconds::zero())
    {
        this->_alarm = std::chrono::high_resolution_clock::now() + this->_cycle;
    }

    return this->_alarm <= std::chrono::high_resolution_clock::now();
}

#endif