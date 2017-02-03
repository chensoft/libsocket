/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/timer.hpp>

// -----------------------------------------------------------------------------
// timer
chen::timer::timer()
{
}

chen::timer::timer(std::function<void ()> cb) : _notify(cb)
{
}

chen::timer::~timer()
{
}

// config
void chen::timer::timeout(const std::chrono::nanoseconds &value)
{
    this->_repeat = false;
    this->_cycle  = value;
    this->_alarm  = std::chrono::high_resolution_clock::time_point();
}

void chen::timer::future(const std::chrono::high_resolution_clock::time_point &value)
{
    this->_repeat = false;
    this->_cycle  = std::chrono::nanoseconds::zero();
    this->_alarm  = value;
}

void chen::timer::interval(const std::chrono::nanoseconds &value)
{
    this->_repeat = true;
    this->_cycle  = value;
    this->_alarm  = std::chrono::high_resolution_clock::time_point();
}

// update
void chen::timer::adjust(const std::chrono::high_resolution_clock::time_point &now)
{
    if (this->_alarm.time_since_epoch() == std::chrono::nanoseconds::zero())
        this->_alarm = now + this->_cycle;
}

bool chen::timer::update(const std::chrono::high_resolution_clock::time_point &now)
{
    auto expired = this->_alarm <= now;

    if (expired && this->_repeat)
        this->_alarm += this->_cycle;

    return expired;
}

// notify
void chen::timer::bind(std::function<void ()> cb)
{
    this->_notify = cb;
}

void chen::timer::emit()
{
    if (this->_notify)
        this->_notify();
}

// event
void chen::timer::onEvent(reactor &loop, int type)
{
    this->emit();
}