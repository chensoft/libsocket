/**
 * Created by Jian Chen
 * @since  2017.01.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/core/reactor.hpp>
#include <chen/base/map.hpp>
#include <chen/sys/sys.hpp>
#include <algorithm>

// -----------------------------------------------------------------------------
// helper
namespace
{
    int po_type(int events)
    {
        // check events, multiple events maybe occur
        if ((events & POLLERR) || (events & POLLHUP))
        {
            return chen::reactor::Closed;
        }
        else
        {
            int ret = 0;

            if (events & POLLIN)
                ret |= chen::reactor::Readable;

            if (events & POLLOUT)
                ret |= chen::reactor::Writable;

            return ret;
        }
    }
}


// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = 0;
const int chen::reactor::FlagOnce = 1;

const int chen::reactor::Readable = 1 << 0;
const int chen::reactor::Writable = 1 << 1;
const int chen::reactor::Closed   = 1 << 2;

chen::reactor::reactor() : reactor(0)  // ignored on Windows
{
}

chen::reactor::reactor(std::size_t count) : _events(count)
{
    // create udp to recv wakeup message
    this->set(&this->_wakeup, nullptr, 0);

    // create udp to allow repoll when user call set or del
    this->set(&this->_repoll, nullptr, 0);
}

chen::reactor::~reactor()
{
    // clear cache before destroy poll
    auto handles = std::move(this->_handles);
    for (auto &pair : handles)
        this->del(pair.second);

    auto timers = std::move(this->_timers);
    for (auto *ptr : timers)
        this->del(ptr);
}

// modify
void chen::reactor::set(basic_handle *ptr, std::function<void(int type)> cb, int mode, int flag)
{
    // register event
    auto find = std::find_if(this->_events.begin(), this->_events.end(), [&] (::pollfd &item) {
        return item.fd == *ptr;
    });

    if (find == this->_events.end())
        find = this->_events.insert(this->_events.end(), ::pollfd());

    find->fd = *ptr;
    find->events = 0;

    if (mode & ModeRead)
        find->events |= POLLIN;

    if (mode & ModeWrite)
        find->events |= POLLOUT;

    // store handle
    this->_handles[*ptr] = ptr;

    // associate callback
    ptr->attach(this, cb, mode, flag);

    // repoll if in polling
    this->_repoll.set();
}

void chen::reactor::set(basic_socket *ptr, std::function<void(int type)> cb, int mode, int flag)
{
    this->set(&ptr->handle(), cb, mode, flag);
}

void chen::reactor::set(event *ptr, std::function<void()> cb, int flag)
{
    this->set(&ptr->handle(), [=](int type) {
        cb();
    }, ModeRead, flag);
}

void chen::reactor::set(timer *ptr, std::function<void()> cb)
{
    ptr->adjust(std::chrono::high_resolution_clock::now());
    ptr->handle().attach(this, [=](int type) {
        cb();
    }, 0, 0);  // mode & flag is useless

    this->_timers.insert(ptr);

    // repoll if in polling
    this->_repoll.set();
}

void chen::reactor::del(basic_handle *ptr)
{
    // clear callback
    ptr->detach();

    // clear handle
    this->_handles.erase(*ptr);

    // delete event
    auto it = std::find_if(this->_events.begin(), this->_events.end(), [&] (::pollfd &item) {
        return item.fd == *ptr;
    });

    if (it != this->_events.end())
        this->_events.erase(it);

    // repoll if in polling
    this->_repoll.set();
}

void chen::reactor::del(basic_socket *ptr)
{
    this->del(&ptr->handle());
}

void chen::reactor::del(event *ptr)
{
    this->del(&ptr->handle());
}

void chen::reactor::del(timer *ptr)
{
    ptr->handle().detach();
    this->_timers.erase(ptr);

    // repoll if in polling
    this->_repoll.set();
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::timed_out) || (code == std::errc::interrupted); code = this->poll())
        ;
}

std::error_code chen::reactor::poll()
{
    return this->poll((std::chrono::nanoseconds::min)());
}

std::error_code chen::reactor::poll(std::chrono::nanoseconds timeout)
{
    // quickly stop
    if (this->_wakeup.signaled())
    {
        this->_wakeup.reset();
        return std::make_error_code(std::errc::operation_canceled);
    }

    // update timer
    auto zero = std::chrono::nanoseconds::zero();
    auto mini = this->update();

    if ((mini >= zero) && (timeout != zero))
        timeout = (timeout > zero) ? (std::min)(mini, timeout) : mini;

    // pull events
    auto error = this->gather(timeout);

    // notify user
    this->notify();

    return error;
}

void chen::reactor::post(basic_handle *ptr, int type, timer *time)
{
    this->_pending.emplace(ptr, type, time);
}

void chen::reactor::stop()
{
    // notify wakeup message via socket
    this->_wakeup.set();
}

// phase
std::chrono::nanoseconds chen::reactor::update()
{
    if (this->_timers.empty())
        return (std::chrono::nanoseconds::min)();

    auto ret = (std::chrono::nanoseconds::min)();
    auto now = std::chrono::high_resolution_clock::now();

    for (auto *ptr : this->_timers)
    {
        auto exp = ptr->update(now);

        if (exp)
        {
            if (ret != std::chrono::nanoseconds::zero())
                ret = std::chrono::nanoseconds::zero();

            this->post(&ptr->handle(), ptr->repeat() ? Readable : Closed, ptr);
        }
        else
        {
            auto off = ptr->alarm() - now;
            if ((off >= std::chrono::nanoseconds::zero()) && ((ret < std::chrono::nanoseconds::zero()) || (ret > off)))
                ret = off;
        }
    }

    return ret;
}

std::error_code chen::reactor::gather(std::chrono::nanoseconds timeout)
{
    // WSAPoll only support millisecond precision and always returned in advance
    if (timeout > std::chrono::nanoseconds::zero())
        timeout += std::chrono::milliseconds(5);

    // pull events
    int result = 0;

    while (true)
    {
        // reset repoll event
        this->_repoll.reset();

        result = ::WSAPoll(this->_events.data(), this->_events.size(), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

        // repoll if user call set or del when polling
        bool repoll = false;

        if (result == 1)
        {
            for (auto it = this->_events.begin(); it != this->_events.end(); ++it)
            {
                if (it->revents && (it->fd == this->_repoll.handle()))
                {
                    repoll = true;
                    break;
                }
            }
        }

        if (!repoll)
            break;
    }

    if (!result)
        return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
    else if (result < 0)
        throw std::system_error(sys::error(), "reactor: failed to poll event");

    // events on the same fd will be notified only once
    for (auto it = this->_events.begin(); it != this->_events.end(); ++it)
    {
        auto &item = *it;
        auto   ptr = chen::map::find(this->_handles, item.fd);

        // continue if revents is 0
        if (!item.revents)
            continue;

        // user request to stop
        if (item.fd == this->_wakeup.handle())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        if (ptr)
            this->post(ptr, po_type(item.events), nullptr);
    }

    return {};
}

void chen::reactor::notify()
{
    while (!this->_pending.empty())
    {
        auto &item = this->_pending.front();
        auto  func = item.ptr->cb();
        auto  type = item.type;

        if ((type & Closed) || (item.ptr->flag() & FlagOnce))
            item.time ? this->del(item.time) : this->del(item.ptr);

        this->_pending.pop();

        func(type);
    }
}

#endif