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
            return chen::ev_base::Closed;

        switch (filter)
        {
            case EVFILT_READ:
                return chen::ev_base::Readable;

            case EVFILT_WRITE:
                return chen::ev_base::Writable;

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

chen::reactor::reactor() : reactor(64)  // 64 is enough
{
}

chen::reactor::reactor(std::size_t count) : _cache(count)
{
    // create kqueue file descriptor
    if ((this->_kqueue = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create kqueue");

    ioctl::cloexec(this->_kqueue, true);

    // create pipe to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
}

chen::reactor::~reactor()
{
    // clear objects before destroy kqueue
    auto objects = std::move(this->_objects);
    for (auto &ptr : objects)
        this->del(ptr);

    auto timers = std::move(this->_timers);
    for (auto *ptr : timers)
        this->del(ptr);

    ::close(this->_kqueue);
}

// modify
void chen::reactor::set(ev_base *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    // register read or delete
    if ((kq_alter(this->_kqueue, fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register write or delete
    if ((kq_alter(this->_kqueue, fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

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

    // delete read
    if ((kq_alter(this->_kqueue, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");

    // delete write
    if ((kq_alter(this->_kqueue, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");
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
    // notify wakeup message via pipe
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
    std::unique_ptr<::timespec> time;

    if (timeout >= std::chrono::nanoseconds::zero())
    {
        auto count = timeout.count();

        time.reset(new ::timespec);
        time->tv_sec  = static_cast<time_t>(count / 1000000000);
        time->tv_nsec = static_cast<time_t>(count % 1000000000);
    }

    int result = 0;

    if ((result = ::kevent(this->_kqueue, nullptr, 0, this->_cache.data(), static_cast<int>(this->_cache.size()), time.get())) <= 0)
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
        auto &item = this->_cache[i];
        auto   ptr = static_cast<ev_base*>(item.udata);

        // user request to stop
        if (ptr == &this->_wakeup)
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        this->post(ptr, kq_type(item.filter, item.flags));
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