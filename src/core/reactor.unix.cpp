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
void chen::reactor::set(ev_handle *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    // register read or delete
    if ((kq_alter(this->_backend, fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register write or delete
    if ((kq_alter(this->_backend, fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, 0, ptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // store handle
    this->_handles.insert(ptr);

    // notify attach
    ptr->onAttach(this, mode, flag);
}

void chen::reactor::del(ev_handle *ptr)
{
    auto fd = ptr->native();

    // notify detach
    ptr->onDetach();

    // clear handle
    this->_handles.erase(ptr);

    // delete read
    if ((kq_alter(this->_backend, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");

    // delete write
    if ((kq_alter(this->_backend, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");
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

    // merge events, events on the same fd will be notified only once
    std::unordered_map<uintptr_t, struct ::kevent*> map;

    for (int i = 0; i < result; ++i)
    {
        auto &item = this->_cache[i];
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
        auto &item = this->_cache[i];
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