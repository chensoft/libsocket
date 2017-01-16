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
    this->_origin = value;
    this->_target = (std::chrono::system_clock::now() + value).time_since_epoch();
}

void chen::timer::interval(const std::chrono::nanoseconds &value)
{
    this->_repeat = true;
    this->_origin = value;
    this->_target = (std::chrono::system_clock::now() + value).time_since_epoch();
}

void chen::timer::future(const std::chrono::nanoseconds &value)
{
    this->_repeat = false;
    this->_origin = (std::chrono::system_clock::now() + value).time_since_epoch();
    this->_target = this->_origin;
}

void chen::timer::future(const std::chrono::system_clock::time_point &value)
{
    this->_repeat = false;
    this->_origin = value.time_since_epoch();
    this->_target = this->_origin;
}

bool chen::timer::expire(const std::chrono::system_clock::time_point &value) const
{
    return value.time_since_epoch() >= this->_target;
}

void chen::timer::update(const std::chrono::system_clock::time_point &value)
{
    // reset target to next timestamp
    if (this->_repeat)
        this->_target = (value + this->_origin).time_since_epoch();
}

#endif