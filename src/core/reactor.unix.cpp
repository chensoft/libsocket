/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

#include <socket/core/reactor.hpp>
#include <socket/core/ioctl.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// helper
namespace
{
    int kq_type(int filter, int flags)
    {
        if ((flags & EV_EOF) || (flags & EV_ERROR))
            return chen::reactor::Closed;

        switch (filter)
        {
            case EVFILT_READ:
                return chen::reactor::Readable;

            case EVFILT_WRITE:
                return chen::reactor::Writable;

            default:
                throw std::runtime_error("reactor: unknown event detect");
        }
    }

    int kq_alter(chen::handle_t kq, chen::handle_t fd, int filter, int flags, uint32_t fflags, intptr_t data, void *udata)
    {
        struct ::kevent event{};
        EV_SET(&event, fd, filter, flags, fflags, data, udata);
        return ::kevent(kq, &event, 1, nullptr, 0, nullptr);
    }
}


// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = EV_CLEAR;
const int chen::reactor::FlagOnce = EV_ONESHOT;

const int chen::reactor::Readable = 1 << 0;
const int chen::reactor::Writable = 1 << 1;
const int chen::reactor::Closed   = 1 << 2;

chen::reactor::reactor() : reactor(64)  // 64 is enough
{
}

chen::reactor::reactor(std::size_t count) : _events(count)
{
    // create kqueue file descriptor
    if ((this->_kqueue = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create kqueue");

    ioctl::cloexec(this->_kqueue, true);

    // create pipe to recv wakeup message
    this->set(&this->_wakeup, nullptr, 0);
}

chen::reactor::~reactor()
{
    // clear cache before destroy kqueue
    auto tmp = std::move(this->_handles);
    for (auto &ev : tmp)
        this->del(ev);

    auto set = std::move(this->_timers);
    for (auto *ptr : set)
        this->del(ptr);

    ::close(this->_kqueue);
}

// modify
void chen::reactor::set(basic_handle *ptr, std::function<void (int type)> cb, int mode, int flag)
{
    // register read or delete
    if ((kq_alter(this->_kqueue, *ptr, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register write or delete
    if ((kq_alter(this->_kqueue, *ptr, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // store handle
    this->_handles.insert(ptr);

    // associate callback
    ptr->attach(this, std::move(cb), mode, flag);
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
    ptr->handle().attach(this, [=] (int type) {
        cb();
    }, 0, 0);  // mode & flag is useless

    this->_timers.emplace_back(ptr);
}

void chen::reactor::del(basic_handle *ptr)
{
    // clear callback
    ptr->detach();

    // clear handle
    this->_handles.erase(ptr);

    // delete read
    if ((kq_alter(this->_kqueue, *ptr, EVFILT_READ, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");

    // delete write
    if ((kq_alter(this->_kqueue, *ptr, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");
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

    auto it = std::remove(this->_timers.begin(), this->_timers.end(), ptr);
    if (it != this->_timers.end())
        this->_timers.erase(it);
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

void chen::reactor::post(basic_handle *ptr, int type, timer *time)
{
    this->_pending.emplace(ptr, type, time);
}

void chen::reactor::stop()
{
    // notify wakeup message via pipe
    this->_wakeup.set();
}

// phase
std::chrono::nanoseconds chen::reactor::update()
{
    // todo
    if (this->_timers.empty())
        return std::chrono::nanoseconds::min();

    std::chrono::nanoseconds ret = std::chrono::nanoseconds::max();

    for (auto *ptr : this->_timers)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto exp = ptr->update(now);

        if (exp)
        {
            ret = std::chrono::nanoseconds::zero();
            this->post(&ptr->handle(), ptr->repeat() ? Readable : Closed, ptr);
            continue;
        }

        ret = std::min(ret, ptr->alarm() - now);
    }

    return ret == std::chrono::nanoseconds::max() ? std::chrono::nanoseconds::min() : ret;
}

std::error_code chen::reactor::gather(std::chrono::nanoseconds timeout)
{
    std::unique_ptr<::timespec> time;

    if (timeout >= std::chrono::nanoseconds::zero())
    {
        auto count = timeout.count();

        time.reset(new ::timespec);
        time->tv_sec  = static_cast<time_t>(count / 1000000000);
        time->tv_nsec = static_cast<time_t>(count % 1000000000);
    }

    int result = 0;

    if ((result = ::kevent(this->_kqueue, nullptr, 0, this->_events.data(), static_cast<int>(this->_events.size()), time.get())) <= 0)
    {
        if (!result)
            return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
        else if (errno == EINTR)
            return std::make_error_code(std::errc::interrupted);  // EINTR maybe triggered by debugger
        else
            throw std::system_error(sys::error(), "reactor: failed to poll event");
    }

    for (int i = 0; i < result; ++i)
    {
        auto &item = this->_events[i];
        auto   ptr = static_cast<basic_handle*>(item.udata);

        // user request to stop
        if (ptr == &this->_wakeup.handle())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        this->post(ptr, kq_type(item.filter, item.flags), nullptr);
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