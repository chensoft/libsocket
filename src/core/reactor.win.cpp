/**
 * Created by Jian Chen
 * @since  2017.01.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include "socket/core/reactor.hpp"
#include "chen/base/map.hpp"
#include "chen/sys/sys.hpp"
#include <winsock.h>
#include <algorithm>

// -----------------------------------------------------------------------------
// helper
namespace chen
{
    typedef struct ::pollfd event_t;

    struct reactor_impl
    {
        // Windows, use WSAPoll
        std::unordered_map<handle_t, ev_handle*> handles;
        std::vector<event_t> cache;
    };

    int po_type(int events)
    {
        // check events, multiple events maybe occur
        if ((events & POLLERR) || (events & POLLHUP))
        {
            return chen::ev_base::Closed;
        }
        else
        {
            int ret = 0;

            if (events & POLLIN)
                ret |= chen::ev_base::Readable;

            if (events & POLLOUT)
                ret |= chen::ev_base::Writable;

            return ret;
        }
    }
}


// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::FlagEdge = 0;
const int chen::reactor::FlagOnce = 1;

chen::reactor::reactor(std::size_t count) : _impl(new reactor_impl)  // count is ignored on Windows
{
    // create udp to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
}

chen::reactor::~reactor()
{
    // clear handles before destroy backend
    auto handles = std::move(this->_impl->handles);
    for (auto &item : handles)
        this->del(item.second);

    auto timers = std::move(this->_timers);
    for (auto *item : timers)
        this->del(item);
}

// modify
void chen::reactor::set(ev_handle *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    // register event
    auto find = std::find_if(this->_impl->cache.begin(), this->_impl->cache.end(), [&] (::pollfd &item) {
        return item.fd == fd;
    });

    if (find == this->_impl->cache.end())
        find = this->_impl->cache.insert(this->_impl->cache.end(), ::pollfd());

    find->fd = fd;
    find->events = 0;

    if (mode & ModeRead)
        find->events |= POLLIN;

    if (mode & ModeWrite)
        find->events |= POLLOUT;

    // store handle
    this->_impl->handles[fd] = ptr;

    // notify attach
    ptr->onAttach(this, mode, flag);
}

void chen::reactor::del(ev_handle *ptr)
{
    auto fd = ptr->native();

    // notify detach
    ptr->onDetach();

    // clear handle
    this->_impl->handles.erase(fd);

    // delete event
    auto it = std::find_if(this->_impl->cache.begin(), this->_impl->cache.end(), [&] (::pollfd &item) {
        return item.fd == fd;
    });

    if (it != this->_impl->cache.end())
        this->_impl->cache.erase(it);
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
    // WSAPoll only support millisecond precision and always returned in advance, so we add a little time
    if (timeout > std::chrono::nanoseconds::zero())
        timeout += std::chrono::milliseconds(5);

    // poll events
    int result = ::WSAPoll(this->_impl->cache.data(), static_cast<ULONG>(this->_impl->cache.size()), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

    if (!result)
        return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
    else if (result < 0)
        throw std::system_error(sys::error(), "reactor: failed to poll event");

    // events on the same fd will be notified only once
    for (auto it = this->_impl->cache.begin(); it != this->_impl->cache.end(); ++it)
    {
        auto &item = *it;
        auto   ptr = chen::map::find(this->_impl->handles, item.fd);

        // continue if revents is 0
        if (!item.revents)
            continue;

        // user request to stop
        if (item.fd == this->_wakeup.native())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        this->post(ptr, po_type(item.revents));
    }

    return {};
}

#endif