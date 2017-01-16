/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/core/timer.hpp>

// -----------------------------------------------------------------------------
// helper
namespace
{
    std::chrono::nanoseconds adjust(const std::chrono::nanoseconds &val)
    {
        // WSAPoll only support millisecond precision, so we do some conversion
        return std::chrono::nanoseconds(val.count() / 1000000 * 1000000);
    }
}


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
    this->_origin = adjust(value);
    this->_target = adjust((std::chrono::system_clock::now() + value).time_since_epoch());
}

void chen::timer::interval(const std::chrono::nanoseconds &value)
{
    this->_repeat = true;
    this->_origin = adjust(value);
    this->_target = adjust((std::chrono::system_clock::now() + value).time_since_epoch());
}

void chen::timer::future(const std::chrono::nanoseconds &value)
{
    this->_repeat = false;
    this->_origin = adjust((std::chrono::system_clock::now() + value).time_since_epoch());
    this->_target = this->_origin;
}

void chen::timer::future(const std::chrono::system_clock::time_point &value)
{
    this->_repeat = false;
    this->_origin = adjust(value.time_since_epoch());
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
        this->_target = adjust((value + this->_origin).time_since_epoch());
}

#endif