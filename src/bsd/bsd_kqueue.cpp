/**
 * Created by Jian Chen
 * @since  2016.09.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/bsd/bsd_kqueue.hpp>
#include <chen/sys/sys.hpp>
#include <sys/event.h>
#include <unistd.h>

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

    // register custom filter to receive the exit message
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
    if (opcode & OpcodeRead)
    {
        EV_SET(&event, fd, EVFILT_READ, EV_ADD | flag, 0, 0, nullptr);

        if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
            throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
    }
    else
    {
        EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);

        if ((::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0) && (errno != ENOENT))
            throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
    }

    // register write or delete
    if (opcode & OpcodeWrite)
    {
        EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | flag, 0, 0, nullptr);

        if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
            throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
    }
    else
    {
        EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

        if ((::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0) && (errno != ENOENT))
            throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
    }
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
chen::bsd::kqueue::Data chen::bsd::kqueue::poll()
{
    struct ::kevent event{};

    // poll next event
    if (::kevent(this->_fd, nullptr, 0, &event, 1, nullptr) != 1)
        throw std::system_error(sys::error(), "kqueue: failed to poll event");

    // check exit status
    if (event.filter == EVFILT_USER)
        return {};

    // return the data
    Data ret;

    ret.fd = static_cast<int>(event.ident);
    ret.ev = this->event(event.filter, event.flags);

    return ret;
}

void chen::bsd::kqueue::exit()
{
    struct ::kevent event{};

    // notify exit message via custom filter
    EV_SET(&event, 0, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);

    if (::kevent(this->_fd, &event, 1, nullptr, 0, nullptr) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to exit the kqueue");
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