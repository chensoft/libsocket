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
const int chen::reactor::FlagEdge = EV_CLEAR;
const int chen::reactor::FlagOnce = EV_ONESHOT;

chen::reactor::reactor(std::size_t count) : _cache(count)
{
    // create kqueue file descriptor
    if ((this->_backend = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create kqueue");

    ioctl::cloexec(this->_backend, true);

    // create pipe to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
}

// modify
void chen::reactor::set(ev_base *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    // register read or delete
    if ((kq_alter(this->_backend, fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register write or delete
    if ((kq_alter(this->_backend, fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
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
    if ((kq_alter(this->_backend, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");

    // delete write
    if ((kq_alter(this->_backend, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");
}

void chen::reactor::del(ev_timer *ptr)
{
    ptr->onDetach();
    this->_timers.erase(ptr);
}

// phase
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

    if ((result = ::kevent(this->_backend, nullptr, 0, this->_cache.data(), static_cast<int>(this->_cache.size()), time.get())) <= 0)
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

#endif