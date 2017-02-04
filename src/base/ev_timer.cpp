/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/ev_timer.hpp>

// -----------------------------------------------------------------------------
// ev_timer
chen::ev_timer::ev_timer()
{
}

chen::ev_timer::ev_timer(std::function<void ()> cb) : _notify(cb)
{
}

chen::ev_timer::~ev_timer()
{
}

// config
void chen::ev_timer::timeout(const std::chrono::nanoseconds &value)
{
    this->_repeat = false;
    this->_cycle  = value;
    this->_alarm  = std::chrono::high_resolution_clock::time_point();
}

void chen::ev_timer::future(const std::chrono::high_resolution_clock::time_point &value)
{
    this->_repeat = false;
    this->_cycle  = std::chrono::nanoseconds::zero();
    this->_alarm  = value;
}

void chen::ev_timer::interval(const std::chrono::nanoseconds &value)
{
    this->_repeat = true;
    this->_cycle  = value;
    this->_alarm  = std::chrono::high_resolution_clock::time_point();
}

// update
void chen::ev_timer::adjust(const std::chrono::high_resolution_clock::time_point &now)
{
    if (this->_alarm.time_since_epoch() == std::chrono::nanoseconds::zero())
        this->_alarm = now + this->_cycle;
}

bool chen::ev_timer::update(const std::chrono::high_resolution_clock::time_point &now)
{
    auto expired = this->_alarm <= now;

    if (expired && this->_repeat)
        this->_alarm += this->_cycle;

    return expired;
}

// notify
void chen::ev_timer::bind(std::function<void ()> cb)
{
    this->_notify = cb;
}

void chen::ev_timer::emit()
{
    if (this->_notify)
        this->_notify();
}

// event
void chen::ev_timer::onEvent(int type)
{
    this->emit();
}