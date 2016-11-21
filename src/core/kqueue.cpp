/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/core/kqueue.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// kqueue
chen::kqueue::kqueue()
{
    // create kqueue file descriptor
    if ((this->_fd = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to create kqueue");

    // register custom filter to receive wake message
    // ident's value is not important here, so use zero is enough
    if (this->alter(0, EVFILT_USER, EV_ADD | EV_CLEAR, 0) < 0)
    {
        ::close(this->_fd);
        throw std::system_error(chen::sys::error(), "kqueue: failed to create custom filter");
    }
}

chen::kqueue::~kqueue()
{
    ::close(this->_fd);
}

// modify
void chen::kqueue::set(handle_t fd, int opcode, int flag)
{
    // register read or delete
    if ((this->alter(fd, EVFILT_READ, (opcode & OpcodeRead) ? EV_ADD | flag : EV_DELETE, 0) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");

    // register write or delete
    if ((this->alter(fd, EVFILT_WRITE, (opcode & OpcodeWrite) ? EV_ADD | flag : EV_DELETE, 0) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
}

void chen::kqueue::del(handle_t fd)
{
    // delete read
    if ((this->alter(fd, EVFILT_READ, EV_DELETE, 0) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");

    // delete write
    if ((this->alter(fd, EVFILT_WRITE, EV_DELETE, 0) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");
}

// poll
std::size_t chen::kqueue::poll(std::vector<Data> &cache, std::size_t count, double timeout)
{
    if (!count)
        return 0;

    // reset wake event
    if (this->alter(0, EVFILT_USER, EV_DISABLE, 0) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to reset the wake event");

    // poll next events
    struct ::kevent events[count];  // VLA
    int result = 0;

    if (timeout >= 0)
    {
        ::timespec val{};
        val.tv_sec  = static_cast<long>(timeout);
        val.tv_nsec = static_cast<long>((timeout - val.tv_sec) * 1000000000);

        result = ::kevent(this->_fd, nullptr, 0, events, static_cast<int>(count), &val);
    }
    else
    {
        result = ::kevent(this->_fd, nullptr, 0, events, static_cast<int>(count), nullptr);
    }

    if (result <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)  // timeout if result is zero
            return 0;
        else
            throw std::system_error(sys::error(), "kqueue: failed to poll event");
    }

    // collect poll data
    auto origin = cache.size();

    for (auto i = 0; i < result; ++i)
    {
        auto &event = events[i];

        // user request to stop
        if (event.filter == EVFILT_USER)
            return 0;

        // remove fd if Ended event occurs
        auto ev = this->event(event.filter, event.flags);
        if (ev == Event::Ended)
            this->del(static_cast<int>(event.ident));

        if (i < origin)
            cache[i] = Data(static_cast<int>(event.ident), ev);
        else
            cache.emplace_back(Data(static_cast<int>(event.ident), ev));
    }

    return static_cast<std::size_t>(result);
}

std::vector<chen::kqueue::Data> chen::kqueue::poll(std::size_t count, double timeout)
{
    std::vector<chen::kqueue::Data> ret;
    this->poll(ret, count, timeout);
    return ret;
}

void chen::kqueue::stop()
{
    // notify wake message via custom filter
    if (this->alter(0, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to stop the kqueue");
}

// misc
chen::kqueue::Event chen::kqueue::event(int filter, int flags)
{
    if (flags & EV_EOF)
        return Event::Ended;

    switch (filter)
    {
        case EVFILT_READ:
            return Event::Readable;

        case EVFILT_WRITE:
            return Event::Writable;

        default:
            throw std::runtime_error("kqueue: unknown event detect");
    }
}

int chen::kqueue::alter(handle_t fd, int filter, int flags, int fflags)
{
    struct ::kevent event{};
    EV_SET(&event, fd, filter, flags, fflags, 0, nullptr);
    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr);
}

#endif