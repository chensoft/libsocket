/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>
#include <memory>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = EV_CLEAR;
const int chen::reactor::FlagOnce = EV_ONESHOT;

chen::reactor::reactor(std::size_t events) : _events(events)
{
    // create kqueue file descriptor
    if ((this->_kqueue = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to create kqueue");

    // register custom filter to receive wake message
    // ident's value is not important here, so use zero is enough
    if (this->alter(0, EVFILT_USER, EV_ADD | EV_CLEAR, 0, nullptr) < 0)
    {
        ::close(this->_kqueue);
        throw std::system_error(chen::sys::error(), "kqueue: failed to create custom filter");
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
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");

    // register write or delete
    if ((this->alter(fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");

    // register callback
    this->_callbacks[fd] = cb;
}

void chen::reactor::del(handle_t fd)
{
    // delete callback
    this->_callbacks.erase(fd);

    // disable pending
    std::for_each(this->_events.begin(), this->_events.end(), [=] (struct ::kevent &event) {
        // user may delete fd in previous callback, if there are events to be
        // processed will lead to errors, so we disable unhandled events here
        if (event.ident == fd)
            event.ident = static_cast<uintptr_t>(invalid_handle);
    });

    // delete read
    if ((this->alter(fd, EVFILT_READ, EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");

    // delete write
    if ((this->alter(fd, EVFILT_WRITE, EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");
}

// run
void chen::reactor::run(double timeout)
{
    while (!this->once(timeout))
        ;
}

bool chen::reactor::once(double timeout)
{
    // poll events
    int result = 0;

    std::unique_ptr<::timespec> val;

    if (timeout >= 0)
    {
        val.reset(new ::timespec);
        val->tv_sec  = static_cast<long>(timeout);
        val->tv_nsec = static_cast<long>((timeout - val->tv_sec) * 1000000000);
    }

    if ((result = ::kevent(this->_kqueue, nullptr, 0, this->_events.data(), static_cast<int>(this->_events.size()), val.get())) <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)  // timeout if result is zero
            return false;
        else
            throw std::system_error(sys::error(), "kqueue: failed to poll event");
    }

    // invoke callback
    for (auto i = 0; i < result; ++i)
    {
        auto &event = this->_events[i];
        auto handle = static_cast<handle_t>(event.ident);

        // disabled by user
        if (handle == invalid_handle)
            continue;

        // user request to stop
        if (event.filter == EVFILT_USER)
            return false;

        auto ev = this->event(event.filter, event.flags);
        auto cb = this->_callbacks.find(handle);

        if (cb != this->_callbacks.end())
            cb->second(handle, ev);
    }

    return result > 0;
}

void chen::reactor::stop()
{
    // notify wake message via custom filter
    if (this->alter(0, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, nullptr) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to stop the kqueue");
}

// misc
chen::reactor::Type chen::reactor::event(int filter, int flags)
{
    if ((flags & EV_EOF) || (flags & EV_ERROR))
        return Type::Closed;

    switch (filter)
    {
        case EVFILT_READ:
            return Type::Readable;

        case EVFILT_WRITE:
            return Type::Writable;

        default:
            throw std::runtime_error("kqueue: unknown event detect");
    }
}

int chen::reactor::alter(handle_t fd, int filter, int flags, int fflags, void *data)
{
    struct ::kevent event{};
    EV_SET(&event, fd, filter, flags, fflags, 0, data);
    return ::kevent(this->_kqueue, &event, 1, nullptr, 0, nullptr);
}

#endif