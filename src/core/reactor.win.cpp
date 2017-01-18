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

chen::reactor::reactor(short count) : _count(count)
{
    // create udp to recv wakeup message
    this->set(&this->_wakeup, nullptr, 0);

    // create udp to allow repoll when user call set or del
    this->set(&this->_repoll, nullptr, 0);
}

chen::reactor::~reactor()
{
    // clear cache before destroy poll
    auto tmp = std::move(this->_cache);
    for (auto &ev : tmp)
        this->del(ev.second);

    auto set = std::move(this->_timers);
    for (auto *ptr : set)
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

    // store event
    this->_cache[*ptr] = ptr;

    // associate callback
    ptr->attach(this, cb, mode, flag);

    // repoll if in polling
    this->_repoll.set();
}

void chen::reactor::set(basic_socket *ptr, std::function<void(int type)> cb, int mode, int flag)
{
    this->set(&ptr->handle(), std::move(cb), mode, flag);
}

void chen::reactor::set(event *ptr, std::function<void()> cb, int flag)
{
    this->set(&ptr->handle(), [=](int type) {
        cb();
    }, ModeRead, flag);
}

void chen::reactor::set(timer *ptr, std::function<void()> cb)
{
    ptr->update(std::chrono::high_resolution_clock::now());
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

    // clear event
    this->_cache.erase(*ptr);

    // delete event
    std::remove_if(this->_events.begin(), this->_events.end(), [&] (::pollfd &item) {
        return item.fd == *ptr;
    });

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
    // calculate the most recent time
    auto zero = std::chrono::nanoseconds::zero();

    if (!this->_timers.empty())
    {
        auto timer = *this->_timers.begin();
        auto value = timer->alarm() - std::chrono::high_resolution_clock::now().time_since_epoch();

        // WSAPoll only support millisecond precision and always returned in advance
        if (timeout < zero)
            timeout = (std::max)(zero, value) + std::chrono::milliseconds(5);
        else
            timeout = (std::min)((std::max)(zero, value), timeout) + std::chrono::milliseconds(5);
    }

    // poll events
    std::vector<::pollfd> events = this->_events;
    int result = 0;

    while (true)
    {
        // reset repoll event
        this->_repoll.reset();

        result = ::WSAPoll(events.data(), events.size(), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

        // repoll if user call set or del when polling
        bool repoll = false;

        if (result == 1)
        {
            for (auto it = events.begin(); it != events.end(); ++it)
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

    if (result < 0)
        throw std::system_error(sys::error(), "reactor: failed to poll event");

    if (!result)
    {
        if (this->update())
            return {};  // success if a timer is triggered

        return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
    }

    // update timer again after poll
    this->update();

    // events on the same fd will be notified only once
    for (auto it = events.begin(); it != events.end(); ++it)
    {
        auto &item = *it;

        // continue if revents is 0
        if (!item.revents)
            continue;

        // user request to stop
        if (item.fd == this->_wakeup.handle())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        // invoke callback
        auto ptr = chen::map::find(this->_cache, item.fd);

        if (ptr)
        {
            auto cb = ptr->callback();
            auto tp = this->type(item.revents);

            if ((tp & Closed) || (ptr->flag() & FlagOnce))
                this->del(ptr);

            if (cb)
                cb(tp);
        }
    }

    return {};
}

void chen::reactor::stop()
{
    // notify wakeup message via socket
    this->_wakeup.set();
}

// misc
int chen::reactor::type(int events)
{
    // check events, multiple events maybe occur
    if ((events & POLLERR) || (events & POLLHUP))
    {
        return Closed;
    }
    else
    {
        int ret = 0;

        if (events & POLLIN)
            ret |= Readable;

        if (events & POLLOUT)
            ret |= Writable;

        return ret;
    }
}

bool chen::reactor::update()
{
    if (this->_timers.empty())
        return false;

    std::vector<timer*> tmp;

    auto now = std::chrono::high_resolution_clock::now();

    for (auto *ptr : this->_timers)
    {
        if (ptr->expired(now))
            tmp.emplace_back(ptr);
        else
            break;
    }

    for (auto *ptr : tmp)
    {
        auto cb = ptr->handle().cb();

        if (ptr->repeat())
        {
            // ptr need reorder because next trigger time is changed
            this->_timers.erase(ptr);
            ptr->update(now);
            this->_timers.insert(ptr);
        }
        else
        {
            this->del(ptr);
        }

        if (cb)
            cb(ModeRead);
    }

    return !tmp.empty();
}

#endif