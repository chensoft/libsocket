/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// helper
namespace
{
    int ep_type(int events)
    {
        // check events, multiple events may occur
        if ((events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP))
        {
            return chen::ev_base::Closed;
        }
        else
        {
            int ret = 0;

            if (events & EPOLLIN)
                ret |= chen::ev_base::Readable;

            if (events & EPOLLOUT)
                ret |= chen::ev_base::Writable;

            return ret;
        }
    }
}


// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = EPOLLET;
const int chen::reactor::FlagOnce = EPOLLONESHOT;

chen::reactor::reactor() : reactor(64)  // 64 is enough
{
}

chen::reactor::reactor(std::size_t count) : _cache(count)
{
    // create epoll file descriptor
    if ((this->_epoll = ::epoll_create1(EPOLL_CLOEXEC)) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create epoll");

    // create eventfd to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
}

chen::reactor::~reactor()
{
    // clear objects before destroy epoll
    auto objects = std::move(this->_objects);
    for (auto &ptr : objects)
        this->del(ptr);

    auto timers = std::move(this->_timers);
    for (auto *ptr : timers)
        this->del(ptr);

    ::close(this->_epoll);
}

// modify
void chen::reactor::set(ev_base *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events  |= flag | EPOLLRDHUP;
    event.data.ptr = ptr;

    // register event
    if (::epoll_ctl(this->_epoll, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_epoll, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "reactor: failed to set event");
    }

    // store object
    this->_objects.insert(ptr);

    // notify attach
    ptr->onAttach(this, mode, flag);
}

void chen::reactor::set(ev_timer *ptr)
{
    this->_timers.insert(ptr);
    ptr->onAttach(this, 0, 0);  // mode & flag are useless
}

void chen::reactor::del(ev_base *ptr)
{
    auto fd = ptr->native();

    // notify detach
    ptr->onDetach();

    // clear object
    this->_objects.erase(ptr);

    // delete event
    if ((::epoll_ctl(this->_epoll, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "reactor: failed to delete event");
}

void chen::reactor::del(ev_timer *ptr)
{
    ptr->onDetach();
    this->_timers.erase(ptr);
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
    // quickly stop
    if (this->_wakeup.signaled())
    {
        this->_wakeup.reset();
        return std::make_error_code(std::errc::operation_canceled);
    }

    // update timer
    auto zero = std::chrono::nanoseconds::zero();
    auto near = this->update();

    if ((near >= zero) && (timeout != zero))
        timeout = (timeout > zero) ? std::min(near, timeout) : near;

    // pull events
    auto error = this->gather(timeout);

    // notify user
    this->notify();

    return error;
}

void chen::reactor::post(ev_base *ptr, int type)
{
    this->_queue.push(std::make_pair(ptr, type));
}

void chen::reactor::post(ev_timer *ptr, int type)
{
    this->post(static_cast<ev_base*>(ptr), type);
}

void chen::reactor::stop()
{
    // notify wakeup message via eventfd
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
        ptr->adjust(now);

        auto exp = ptr->update(now);

        if (exp)
        {
            if (ret != std::chrono::nanoseconds::zero())
                ret = std::chrono::nanoseconds::zero();

            this->post(ptr, ptr->repeat() ? ev_base::Readable : ev_base::Closed);
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
    // pull events
    int result = ::epoll_wait(this->_epoll, this->_cache.data(), static_cast<int>(this->_cache.size()), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

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
        auto &item = this->_cache[i];
        auto   ptr = static_cast<ev_base*>(item.data.ptr);

        // user request to stop
        if (ptr == &this->_wakeup)
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        this->post(ptr, ep_type(item.events));
    }

    return {};
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

#endif