/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = EV_CLEAR;
const int chen::reactor::FlagOnce = EV_ONESHOT;

chen::reactor::reactor()
{
    // create kqueue file descriptor
    if ((this->_fd = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to create kqueue");

    // register custom filter to receive wake message
    // ident's value is not important here, so use zero is enough
    if (this->alter(0, EVFILT_USER, EV_ADD | EV_CLEAR, 0, nullptr) < 0)
    {
        ::close(this->_fd);
        throw std::system_error(chen::sys::error(), "kqueue: failed to create custom filter");
    }
}

chen::reactor::~reactor()
{
    ::close(this->_fd);
}

// modify
void chen::reactor::set(handle_t fd, void *data, int mode, int flag)
{
    // register read or delete
    if ((this->alter(fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, data) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");

    // register write or delete
    if ((this->alter(fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, data) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");
}

void chen::reactor::del(handle_t fd)
{
    // delete read
    if ((this->alter(fd, EVFILT_READ, EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");

    // delete write
    if ((this->alter(fd, EVFILT_WRITE, EV_DELETE, 0, nullptr) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to delete event");
}

// run
void chen::reactor::run(size_t count)
{
    // todo
//    while (true)
//    {
//        auto list = this->poll(count);
//        if (list.empty())
//            break;  // user request to stop or timeout
//
//        for (auto &item : list)
//        {
//            switch (item.type)
//            {
//                case Type::Readable:
//                    static_cast<reactor_delegate*>(item.data)->onReadable();
//                    break;
//
//                case Type::Writable:
//                    static_cast<reactor_delegate*>(item.data)->onWritable();
//                    break;
//
//                case Type::Closed:
//                    static_cast<reactor_delegate*>(item.data)->onEnded();
//                    break;
//            }
//        }
//    }
}

void chen::reactor::stop()
{
    // notify wake message via custom filter
    if (this->alter(0, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, nullptr) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to stop the kqueue");
}

// poll
std::vector<chen::reactor::Data> chen::reactor::poll(std::size_t count, double timeout)
{
    if (!count)
        return {};

    // poll next events
    struct ::kevent events[count];  // VLA
    int result = 0;

    std::unique_ptr<::timespec> val;

    if (timeout >= 0)
    {
        val.reset(new ::timespec);
        val->tv_sec  = static_cast<long>(timeout);
        val->tv_nsec = static_cast<long>((timeout - val->tv_sec) * 1000000000);
    }

    if ((result = ::kevent(this->_fd, nullptr, 0, events, static_cast<int>(count), val.get())) <= 0)
    {
        // EINTR maybe triggered by debugger, treat it as user request to stop
        if ((errno == EINTR) || !result)  // timeout if result is zero
            return {};
        else
            throw std::system_error(sys::error(), "kqueue: failed to poll event");
    }

    // collect poll data
    std::vector<chen::reactor::Data> ret;

    for (auto i = 0; i < result; ++i)
    {
        auto &event = events[i];

        // user request to stop
        if (event.filter == EVFILT_USER)
            return {};

        auto ev = this->event(event.filter, event.flags);
        ret.emplace_back(Data(event.udata, ev));
    }

    return ret;
}

// misc
chen::reactor::Type chen::reactor::event(int filter, int flags)
{
    if (flags & EV_EOF)
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
    return ::kevent(this->_fd, &event, 1, nullptr, 0, nullptr);
}

#endif