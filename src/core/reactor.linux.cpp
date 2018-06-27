/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include "socket/core/reactor.hpp"
#include "chen/sys/sys.hpp"
#include <sys/epoll.h>
#include <unistd.h>

// -----------------------------------------------------------------------------
// helper
namespace chen
{
    typedef struct ::epoll_event event_t;

    struct reactor_impl
    {
        reactor_impl(std::size_t count) : cache(count) {}

        // Linux, use epoll
        handle_t backend = invalid_handle;
        std::unordered_set<ev_handle*> handles;
        std::vector<event_t> cache;
    };

    int ep_type(int events)
    {
        // check events, multiple events may occur
        if ((events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP))
        {
            return chen::ev_base::Closed;
        }
        else
        {
            int ret = 0;

            if (events & EPOLLIN)
                ret |= chen::ev_base::Readable;

            if (events & EPOLLOUT)
                ret |= chen::ev_base::Writable;

            return ret;
        }
    }
}


// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = EPOLLET;
const int chen::reactor::FlagOnce = EPOLLONESHOT;

chen::reactor::reactor(std::size_t count) : _impl(new reactor_impl(count))
{
    // create epoll file descriptor
    if ((this->_impl->backend = ::epoll_create1(EPOLL_CLOEXEC)) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create epoll");

    // create eventfd to recv wakeup message
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

    // register event
    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events  |= flag | EPOLLRDHUP;
    event.data.ptr = ptr;

    if (::epoll_ctl(this->_impl->backend, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_impl->backend, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "reactor: failed to set event");
    }

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

    // delete event
    if ((::epoll_ctl(this->_impl->backend, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "reactor: failed to delete event");
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
    // poll events
    int result = ::epoll_wait(this->_impl->backend, this->_impl->cache.data(), static_cast<int>(this->_impl->cache.size()), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

    if (result <= 0)
    {
        if (!result)
            return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
        else if (errno == EINTR)
            return std::make_error_code(std::errc::interrupted);  // EINTR maybe triggered by debugger
        else
            throw std::system_error(sys::error(), "reactor: failed to poll event");
    }

    // epoll has helped us merge the events
    // events on the same fd will be notified only once
    for (int i = 0; i < result; ++i)
    {
        auto &item = this->_impl->cache[i];
        auto   ptr = static_cast<ev_handle*>(item.data.ptr);

        // user request to stop
        if (ptr == &this->_wakeup)
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        this->post(ptr, ep_type(item.events));
    }

    return {};
}

#endif