/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/core/reactor.hpp>
#include <chen/base/map.hpp>
#include <chen/sys/sys.hpp>
#include <memory>

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

chen::reactor::reactor(std::uint8_t count) : _count(count)
{
    // create kqueue file descriptor
    if ((this->_kqueue = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create kqueue");

    // register custom filter to recv wakeup message
    // ident's value is not important here, use zero is ok
    if (this->alter(0, EVFILT_USER, EV_ADD, 0, nullptr) < 0)
    {
        ::close(this->_kqueue);
        throw std::system_error(chen::sys::error(), "reactor: failed to create custom filter");
    }
}

chen::reactor::~reactor()
{
    ::close(this->_kqueue);
}

// modify
void chen::reactor::set(handle_t fd, callback cb, int mode, int flag)
{
    // register read or delete
    if ((this->alter(fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register write or delete
    if ((this->alter(fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to set event");

    // register callback
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_calls[fd] = cb;
}

void chen::reactor::del(handle_t fd)
{
    // delete callback
    {
        std::lock_guard<std::mutex> lock(this->_mutex);
        this->_calls.erase(fd);
    }

    // delete read
    if ((this->alter(fd, EVFILT_READ, EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");

    // delete write
    if ((this->alter(fd, EVFILT_WRITE, EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "reactor: failed to delete event");
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::interrupted); code = this->poll())
        ;
}

std::error_code chen::reactor::poll(double timeout)
{
    // poll events
    struct ::kevent events[this->_count];  // VLA
    std::unique_ptr<::timespec> val;

    if (timeout >= 0)
    {
        val.reset(new ::timespec);
        val->tv_sec  = static_cast<time_t>(timeout);
        val->tv_nsec = static_cast<time_t>((timeout - val->tv_sec) * 1000000000);
    }

    int result = 0;

    if ((result = ::kevent(this->_kqueue, nullptr, 0, events, this->_count, val.get())) <= 0)
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
        auto &item = events[i];

        if (item.filter == EVFILT_USER)
            continue;

        auto find = map.find(item.ident);
        auto type = this->type(item.filter, item.flags);

        if (find != map.end())
        {
            item.ident = static_cast<uintptr_t>(invalid_handle);  // set to invalid because we merge item's event to previous item
            find->second->filter |= type;  // borrow 'filter' field for temporary use
        }
        else
        {
            map[item.ident] = &item;
            item.filter = static_cast<std::int16_t>(type);  // filter is enough to store event type
        }
    }

    // invoke callback
    for (int i = 0; i < result; ++i)
    {
        auto &item = events[i];
        auto    fd = static_cast<handle_t>(item.ident);

        // user request to stop
        if (item.filter == EVFILT_USER)
            return std::make_error_code(std::errc::operation_canceled);

        // item is invalid
        if (fd == invalid_handle)
            continue;

        // normal callback
        callback cb;

        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            cb = chen::map::find(this->_calls, fd);
        }

        if (cb)
            cb(item.filter);
    }

    return {};
}

void chen::reactor::stop()
{
    // notify wakeup message via custom filter
    if (this->alter(0, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, nullptr) < 0)
        throw std::system_error(sys::error(), "reactor: failed to stop the kqueue");
}

// misc
int chen::reactor::type(int filter, int flags)
{
    if ((flags & EV_EOF) || (flags & EV_ERROR))
        return Closed;

    switch (filter)
    {
        case EVFILT_READ:
            return Readable;

        case EVFILT_WRITE:
            return Writable;

        default:
            throw std::runtime_error("reactor: unknown event detect");
    }
}

int chen::reactor::alter(handle_t fd, int filter, int flags, int fflags, void *data)
{
    struct ::kevent event{};
    EV_SET(&event, fd, filter, flags, fflags, 0, data);
    return ::kevent(this->_kqueue, &event, 1, nullptr, 0, nullptr);
}

#endif