/**
 * Created by Jian Chen
 * @since  2017.01.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/core/reactor.hpp>
#include <chen/base/map.hpp>
#include <chen/sys/sys.hpp>
#include <algorithm>

// -----------------------------------------------------------------------------
// helper
namespace
{
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

chen::reactor::reactor(std::size_t count)  // count is ignored on Windows
{
    // create udp to recv wakeup message
    this->set(&this->_wakeup, ModeRead, 0);
}

// modify
void chen::reactor::set(ev_handle *ptr, int mode, int flag)
{
    auto fd = ptr->native();

    // register event
    auto find = std::find_if(this->_cache.begin(), this->_cache.end(), [&] (::pollfd &item) {
        return item.fd == fd;
    });

    if (find == this->_cache.end())
        find = this->_cache.insert(this->_cache.end(), ::pollfd());

    find->fd = fd;
    find->events = 0;

    if (mode & ModeRead)
        find->events |= POLLIN;

    if (mode & ModeWrite)
        find->events |= POLLOUT;

    // store handle
    this->_handles[fd] = ptr;

    // notify attach
    ptr->onAttach(this, mode, flag);
}

void chen::reactor::del(ev_handle *ptr)
{
    auto fd = ptr->native();

    // notify detach
    ptr->onDetach();

    // clear handle
    this->_handles.erase(fd);

    // delete event
    auto it = std::find_if(this->_cache.begin(), this->_cache.end(), [&] (::pollfd &item) {
        return item.fd == fd;
    });

    if (it != this->_cache.end())
        this->_cache.erase(it);
}

// phase
std::error_code chen::reactor::gather(std::chrono::nanoseconds timeout)
{
    // WSAPoll only support millisecond precision and always returned in advance, so we add a little time
    if (timeout > std::chrono::nanoseconds::zero())
        timeout += std::chrono::milliseconds(5);

    // poll events
    int result = ::WSAPoll(this->_cache.data(), this->_cache.size(), timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

    if (!result)
        return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
    else if (result < 0)
        throw std::system_error(sys::error(), "reactor: failed to poll event");

    // events on the same fd will be notified only once
    for (auto it = this->_cache.begin(); it != this->_cache.end(); ++it)
    {
        auto &item = *it;
        auto   ptr = chen::map::find(this->_handles, item.fd);

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