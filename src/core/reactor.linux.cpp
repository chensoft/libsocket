/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include "socket/core/reactor.hpp"
#include "chen/sys/sys.hpp"

// -----------------------------------------------------------------------------
// helper
namespace
{
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

chen::reactor::reactor(std::size_t count) : _cache(count)
{
    // create epoll file descriptor
    if ((this->_backend = ::epoll_create1(EPOLL_CLOEXEC)) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create epoll");

    // create eventfd to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
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

    if (::epoll_ctl(this->_backend, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_backend, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "reactor: failed to set event");
    }

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

    // delete event
    if ((::epoll_ctl(this->_backend, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "reactor: failed to delete event");
}

// phase
std::error_code chen::reactor::gather(std::chrono::nanoseconds timeout)
{
    // poll events
    int result = ::epoll_wait(this->_backend, this->_cache.data(), static_cast<int>(this->_cache.size()), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

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
        auto &item = this->_cache[i];
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