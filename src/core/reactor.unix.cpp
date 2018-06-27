/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

#include "socket/core/reactor.hpp"
#include "socket/core/ioctl.hpp"
#include "chen/sys/sys.hpp"
#include <sys/event.h>
#include <unistd.h>

// -----------------------------------------------------------------------------
// helper
namespace chen
{
    typedef struct ::kevent event_t;

    struct reactor_impl
    {
        reactor_impl(std::size_t count) : cache(count) {}

        // Unix, use kqueue
        handle_t backend = invalid_handle;
        std::unordered_set<ev_handle*> handles;
        std::vector<event_t> cache;
    };

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

chen::reactor::reactor(std::size_t count) : _impl(new reactor_impl(count))
{
    // create kqueue file descriptor
    if ((this->_impl->backend = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create kqueue");

    ioctl::cloexec(this->_impl->backend, true);

    // create pipe to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
}

chen::reactor::~reactor()
{
    // clear handles before destroy backend
    auto handles = std::move(this->_impl->handles);
    for (auto &item : handles)
        this->del(item);

    ::close(this->_impl->backend);

    auto timers = std::move(this->_timers);
    for (auto *item : timers)
        this->del(item);
}

// modify
void chen::reactor::set(ev_handle *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    // register read or delete
    if ((kq_alter(this->_impl->backend, fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register write or delete
    if ((kq_alter(this->_impl->backend, fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // store handle
    this->_impl->handles.insert(ptr);

    // notify attach
    ptr->onAttach(this, mode, flag);
}

void chen::reactor::del(ev_handle *ptr)
{
    auto fd = ptr->native();

    // notify detach
    ptr->onDetach();

    // clear handle
    this->_impl->handles.erase(ptr);

    // delete read
    if ((kq_alter(this->_impl->backend, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");

    // delete write
    if ((kq_alter(this->_impl->backend, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");
}

// run
void chen::reactor::run()
{
    // quit if no events to monitor or operation canceled
    for (std::error_code code; ((this->_impl->handles.size() > 1) || !this->_timers.empty()) && (code != std::errc::operation_canceled); code = this->poll())
        ;
}

// phrase
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

    if ((result = ::kevent(this->_impl->backend, nullptr, 0, this->_impl->cache.data(), static_cast<int>(this->_impl->cache.size()), time.get())) <= 0)
    {
        if (!result)
            return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
        else if (errno == EINTR)
            return std::make_error_code(std::errc::interrupted);  // EINTR maybe triggered by debugger
        else
            throw std::system_error(sys::error(), "reactor: failed to poll event");
    }

    // merge events, events on the same fd will be notified only once
    std::unordered_map<uintptr_t, struct ::kevent*> map;

    for (int i = 0; i < result; ++i)
    {
        auto &item = this->_impl->cache[i];
        auto  find = map.find(item.ident);
        auto  type = kq_type(item.filter, item.flags);

        if (find != map.end())
        {
            item.udata = nullptr;  // set to null because we merge item's event to previous item
            find->second->filter |= type;  // borrow 'filter' field for temporary use
        }
        else
        {
            map[item.ident] = &item;
            item.filter = static_cast<std::int16_t>(type);  // filter is enough to store event type
        }
    }

    for (int i = 0; i < result; ++i)
    {
        auto &item = this->_impl->cache[i];
        auto   ptr = static_cast<ev_handle*>(item.udata);

        // user request to stop
        if (ptr == &this->_wakeup)
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        if (ptr)
            this->post(ptr, item.filter);
    }

    return {};
}

#endif