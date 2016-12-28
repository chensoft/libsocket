/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if !defined(__linux__) && !defined(_WIN32)

#include <socket/core/reactor.hpp>
#include <chen/sys/sys.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = EV_CLEAR;
const int chen::reactor::FlagOnce = EV_ONESHOT;

chen::reactor::reactor(int count) : _count(count)
{
    // create kqueue file descriptor
    if ((this->_kqueue = ::kqueue()) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to create kqueue");

    // register custom filter to receive wake message
    // ident's value is not important here, so use zero is ok
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
void chen::reactor::set(handle_t fd, callback *cb, int mode, int flag)
{
    // register read or delete
    if ((this->alter(fd, EVFILT_READ, (mode & ModeRead) ? EV_ADD | flag : EV_DELETE, 0, cb) < 0) && (errno != ENOENT))
        throw std::system_error(chen::sys::error(), "kqueue: failed to set event");

    // register write or delete
    if ((this->alter(fd, EVFILT_WRITE, (mode & ModeWrite) ? EV_ADD | flag : EV_DELETE, 0, cb) < 0) && (errno != ENOENT))
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
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::interrupted); code = this->once())
        ;
}

std::error_code chen::reactor::once(double timeout)
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
            throw std::system_error(sys::error(), "kqueue: failed to poll event");
    }

    // merge events, events on the same fd will be notified only once
    typedef std::vector<std::pair<callback*, Type>> merge_t;  // keep events' original order
    typedef std::unordered_map<callback*, merge_t::iterator> cache_t;

    merge_t merge;
    cache_t cache;

    for (int i = 0; i < result; ++i)
    {
        auto &item = events[i];
        auto *call = static_cast<callback*>(item.udata);

        // user request to stop
        if (item.filter == EVFILT_USER)
        {
            // store for later use
            merge.emplace_back(std::make_pair(nullptr, 0));
            continue;
        }

        if (!call)
            continue;

        // find exist event
        auto find = cache.find(call);
        auto type = this->type(item.filter, item.flags);

        if (find != cache.end())
            find->second->second |= type;
        else
            cache[call] = merge.insert(merge.end(), std::make_pair(call, type));
    }

    // invoke callback
    for (auto it = merge.begin(); it != merge.end(); ++it)
    {
        // user request to stop
        if (!it->first)
            return std::make_error_code(std::errc::operation_canceled);

        (*it->first)(it->second);
    }

    return {};
}

void chen::reactor::stop()
{
    // notify wake message via custom filter
    if (this->alter(0, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, nullptr) < 0)
        throw std::system_error(sys::error(), "kqueue: failed to stop the kqueue");
}

// misc
chen::reactor::Type chen::reactor::type(int filter, int flags)
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