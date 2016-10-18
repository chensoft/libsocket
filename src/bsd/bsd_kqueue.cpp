/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/bsd/bsd_kqueue.hpp>
#include <socket/config.hpp>
#include <chen/sys/sys.hpp>
#include <sys/event.h>

// -----------------------------------------------------------------------------
// kqueue
const int chen::bsd::kqueue::FlagOnce = EV_ONESHOT;
const int chen::bsd::kqueue::FlagEdge = EV_CLEAR;

chen::bsd::kqueue::kqueue()
{
    // create kqueue file descriptor
    if ((this->_fd = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to create kqueue");

    struct ::kevent event{};

    // register custom filter to receive wake message
    // ident's value is not important in this case, so use zero is enough
    EV_SET(&event, 0, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, nullptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
    {
        ::close(this->_fd);
        throw std::system_error(chen::sys::error(), "kqueue: failed to create custom filter");
    }
}

chen::bsd::kqueue::~kqueue()
{
    ::close(this->_fd);
}

// modify
void chen::bsd::kqueue::set(int fd, int opcode, int flag)
{
    struct ::kevent event{};

    // register read or delete
    EV_SET(&event, fd, EVFILT_READ, (opcode & OpcodeRead) ? EV_ADD | flag : EV_DELETE, 0, 0, nullptr);

    if ((::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");

    // register write or delete
    EV_SET(&event, fd, EVFILT_WRITE, (opcode & OpcodeWrite) ? EV_ADD | flag : EV_DELETE, 0, 0, nullptr);

    if ((::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
}

void chen::bsd::kqueue::del(int fd)
{
    struct ::kevent event{};

    // delete read
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);

    if ((::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");

    // delete write
    EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

    if ((::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");
}

// poll
std::size_t chen::bsd::kqueue::poll(std::vector<Data> &cache, std::size_t count, double timeout)
{
    if (!count)
        return 0;

    struct ::kevent events[count];
    int result = 0;

    // poll next events
    if (timeout < 0.0)
    {
        if ((result = ::kevent(this->_fd, nullptr, 0, events, static_cast<int>(count), nullptr)) <= 0)
            throw std::system_error(sys::error(), "kqueue: failed to poll event");
    }
    else
    {
        struct timespec val{};
        val.tv_sec  = static_cast<long>(timeout);
        val.tv_nsec = static_cast<long>((timeout - val.tv_sec) * 1000000000);

        if ((result = ::kevent(this->_fd, nullptr, 0, events, static_cast<int>(count), &val)) < 0)
            throw std::system_error(sys::error(), "kqueue: failed to poll event");
        else if (!result)
            return 0;  // timeout
    }

    // check return data
    auto length = cache.size();

    for (std::size_t i = 0; i < static_cast<std::size_t>(result); ++i)
    {
        auto &event = events[i];

        if (event.filter == EVFILT_USER)
        {
            // user request to stop
            return 0;
        }
        else
        {
            if (i < length)
                cache[i] = Data(static_cast<int>(event.ident), this->event(event.filter, event.flags));
            else
                cache.emplace_back(Data(static_cast<int>(event.ident), this->event(event.filter, event.flags)));
        }
    }

    return static_cast<std::size_t>(result);
}

std::vector<chen::bsd::kqueue::Data> chen::bsd::kqueue::poll(std::size_t count, double timeout)
{
    std::vector<chen::bsd::kqueue::Data> ret;
    this->poll(ret, count, timeout);
    return ret;
}

void chen::bsd::kqueue::stop()
{
    struct ::kevent event{};

    // notify wake message via custom filter
    EV_SET(&event, 0, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to stop the kqueue");
}

// misc
chen::bsd::kqueue::Event chen::bsd::kqueue::event(int filter, int flags)
{
    if (flags & EV_EOF)
        return Event::End;

    switch (filter)
    {
        case EVFILT_READ:
            return Event::Read;

        case EVFILT_WRITE:
            return Event::Write;

        default:
            throw std::runtime_error("kqueue: unknown event detect");
    }
}

#endif