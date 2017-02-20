/**
 * Created by Jian Chen
 * @since  2017.02.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

chen::reactor::reactor() : reactor(64)  // 64 is enough
{
}

chen::reactor::~reactor()
{
    // clear handles before destroy backend
#ifdef _WIN32
    auto handles = std::move(this->_handles);
    for (auto &item : handles)
        this->del(item.second);
#else
    auto handles = std::move(this->_handles);
    for (auto &item : handles)
        this->del(item);

    ::close(this->_backend);
#endif

    auto timers = std::move(this->_timers);
    for (auto *item : timers)
        this->del(item);
}

// modify
void chen::reactor::set(ev_timer *ptr, std::chrono::high_resolution_clock::time_point init)
{
    ptr->setup(init);
    this->_timers.insert(ptr);
    ptr->onAttach(this, 0, 0);  // mode & flag are useless
}

void chen::reactor::del(ev_timer *ptr)
{
    ptr->onDetach();
    this->_timers.erase(ptr);
}

// run
void chen::reactor::run()
{
    // quit if no events to monitor or operation canceled
    for (std::error_code code; ((this->_handles.size() > 1) || !this->_timers.empty()) && (code != std::errc::operation_canceled); code = this->poll())
        ;
}

std::error_code chen::reactor::poll()
{
    return this->poll(std::chrono::nanoseconds::min());
}

std::error_code chen::reactor::poll(std::chrono::nanoseconds timeout)
{
    // update timer
    auto zero = std::chrono::nanoseconds::zero();
    auto mini = this->update();

    if ((mini >= zero) && (timeout != zero))
        timeout = (timeout > zero) ? std::min(mini, timeout) : mini;

    // poll events
    auto error = this->gather(timeout);

    // notify user
    this->notify();

    // quickly stop
    if (this->_wakeup.signaled())
    {
        this->_wakeup.reset();
        return std::make_error_code(std::errc::operation_canceled);
    }

    return error;
}

void chen::reactor::post(ev_handle *ptr, int type)
{
    this->_queue.emplace(ptr, type);
}

void chen::reactor::post(ev_timer *ptr)
{
    this->_queue.emplace(ptr, 0);
}

void chen::reactor::stop()
{
    // notify wakeup message
    this->_wakeup.set();
}

// phase
std::chrono::nanoseconds chen::reactor::update()
{
    if (this->_timers.empty())
        return std::chrono::nanoseconds::min();

    auto ret = std::chrono::nanoseconds::min();
    auto now = std::chrono::high_resolution_clock::now();

    for (auto *ptr : this->_timers)
    {
        // the front is the nearest timer
        if (ptr->expire(now))
        {
            if (ptr->repeat())
                ptr->update(now);

            // don't wait for the following backend event if we have a callback need to notify
            if (ret.count())
                ret = std::chrono::nanoseconds::zero();

            this->post(ptr);
        }
        else
        {
            if (ret.count())
                ret = ptr->alarm() - now;

            break;
        }
    }

    return ret;
}

void chen::reactor::notify()
{
    while (!this->_queue.empty())
    {
        auto item = this->_queue.front();
        this->_queue.pop();

        item.first->onEvent(item.second);
    }
}