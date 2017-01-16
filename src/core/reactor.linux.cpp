/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>
#include <limits>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = EPOLLET;
const int chen::reactor::FlagOnce = EPOLLONESHOT;

const int chen::reactor::Readable = 1 << 0;
const int chen::reactor::Writable = 1 << 1;
const int chen::reactor::Closed   = 1 << 2;

chen::reactor::reactor() : reactor(64)  // 64 is enough
{
}

chen::reactor::reactor(short count) : _count(count)
{
    // create epoll file descriptor
    if ((this->_epoll = ::epoll_create1(EPOLL_CLOEXEC)) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create epoll");

    // create eventfd to recv wakeup message
    this->set(&this->_wakeup, nullptr, 0);
}

chen::reactor::~reactor()
{
    // clear cache before destroy epoll
    auto tmp = std::move(this->_cache);
    for (auto &ev : tmp)
        this->del(ev);

    ::close(this->_epoll);
}

// modify
void chen::reactor::set(basic_handle *ptr, std::function<void (int type)> cb, int mode, int flag)
{
    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events  |= flag | EPOLLRDHUP;
    event.data.ptr = ptr;

    // register event
    if (::epoll_ctl(this->_epoll, EPOLL_CTL_MOD, *ptr, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_epoll, EPOLL_CTL_ADD, *ptr, &event) != 0))
            throw std::system_error(sys::error(), "reactor: failed to set event");
    }

    // store event
    this->_cache.insert(ptr);

    // associate callback
    ptr->attach(this, cb, mode, flag);
}

void chen::reactor::set(basic_socket *ptr, std::function<void (int type)> cb, int mode, int flag)
{
    this->set(&ptr->handle(), std::move(cb), mode, flag);
}

void chen::reactor::set(event *ptr, std::function<void ()> cb, int flag)
{
    this->set(&ptr->handle(), [=] (int type) {
        cb();
    }, ModeRead, flag);
}

void chen::reactor::set(timer *ptr, std::function<void ()> cb)
{
    this->set(&ptr->handle(), [=] (int type) {
        cb();
    }, ModeRead, 0);
}

void chen::reactor::del(basic_handle *ptr)
{
    // clear callback
    ptr->detach();

    // clear event
    this->_cache.erase(ptr);

    // delete event
    if ((::epoll_ctl(this->_epoll, EPOLL_CTL_DEL, *ptr, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "reactor: failed to delete event");
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
    this->del(&ptr->handle());
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::timed_out) || (code == std::errc::interrupted); code = this->poll())
        ;
}

std::error_code chen::reactor::poll()
{
    return this->poll(std::chrono::nanoseconds::min());
}

std::error_code chen::reactor::poll(std::chrono::nanoseconds timeout)
{
    // poll events
    ::epoll_event events[this->_count];  // VLA
    int result = ::epoll_wait(this->_epoll, events, this->_count, timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

    if (result <= 0)
    {
        if (!result)
            return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
        else if (errno == EINTR)
            return std::make_error_code(std::errc::interrupted);  // EINTR maybe triggered by debugger
        else
            throw std::system_error(sys::error(), "reactor: failed to poll event");
    }

    // epoll has helped us merge the events
    // events on the same fd will be notified only once
    for (int i = 0; i < result; ++i)
    {
        auto &item = events[i];
        auto   ptr = static_cast<basic_handle*>(item.data.ptr);

        // user request to stop
        if (ptr == &this->_wakeup.handle())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        // normal callback
        if (ptr)
        {
            auto cb = ptr->callback();
            auto tp = this->type(item.events);

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
    // notify wakeup message via eventfd
    this->_wakeup.set();
}

// misc
int chen::reactor::type(int events)
{
    // check events, multiple events maybe occur
    if ((events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP))
    {
        return Closed;
    }
    else
    {
        int ret = 0;

        if (events & EPOLLIN)
            ret |= Readable;

        if (events & EPOLLOUT)
            ret |= Writable;

        return ret;
    }
}

#endif