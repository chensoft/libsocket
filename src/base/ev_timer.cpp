/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/base/ev_timer.hpp>
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// ev_timer
chen::ev_timer::ev_timer(std::function<void ()> cb) : _notify(std::move(cb))
{
}

// config
void chen::ev_timer::timeout(const std::chrono::nanoseconds &value)
{
    this->_flag = Flag::Normal;
    this->_time = value;
    this->_when = std::chrono::steady_clock::time_point();
}

void chen::ev_timer::future(const std::chrono::nanoseconds &value)
{
    this->future(std::chrono::steady_clock::now() + value);
}

void chen::ev_timer::future(const std::chrono::steady_clock::time_point &value)
{
    this->_flag = Flag::Future;
    this->_time = std::chrono::nanoseconds::zero();
    this->_when = value;
}

void chen::ev_timer::interval(const std::chrono::nanoseconds &value)
{
    this->_flag = Flag::Repeat;
    this->_time = value;
    this->_when = std::chrono::steady_clock::time_point();
}

// notify
void chen::ev_timer::attach(std::function<void ()> cb)
{
    this->_notify = std::move(cb);
}

// update
void chen::ev_timer::setup(const std::chrono::steady_clock::time_point &now)
{
    if (this->_flag == Flag::Future)
        this->_time = this->_when - now;  // in order to compare in multiset, no other use
    else
        this->_when = now + this->_time;
}

// event
void chen::ev_timer::onEvent(int type)
{
    auto loop = this->evLoop();
    auto func = this->_notify;

    if (loop && (this->_flag != Flag::Repeat))
        loop->del(this);

    if (func)
        func();
}