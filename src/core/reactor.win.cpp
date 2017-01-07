/**
 * Created by Jian Chen
 * @since  2017.01.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <socket/inet/inet_address.hpp>
#include <socket/core/reactor.hpp>
#include <chen/base/map.hpp>
#include <chen/sys/sys.hpp>
#include <algorithm>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = 0;
const int chen::reactor::FlagOnce = 1;

const int chen::reactor::Readable = 1 << 0;
const int chen::reactor::Writable = 1 << 1;
const int chen::reactor::Closed   = 1 << 2;

chen::reactor::reactor(std::uint8_t count) : _count(count)
{
    // create udp to recv wakeup message
    this->set(this->_wakeup.native(), nullptr, ModeRead, 0);

    // create udp to allow repoll when user call set or del
    this->set(this->_repoll.native(), nullptr, ModeRead, 0);
}

chen::reactor::~reactor()
{
}

// modify
void chen::reactor::set(handle_t fd, callback cb, int mode, int flag)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    // register event
    auto find = std::find_if(this->_cache.begin(), this->_cache.end(), [=] (::pollfd &item) {
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

    this->_flags[fd] = flag;

    // register callback
    this->_calls[fd] = cb;

    // repoll if in polling
    this->_repoll.set();
}

void chen::reactor::del(handle_t fd)
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    
    // delete callback
    this->_calls.erase(fd);

    // delete flags
    this->_flags.erase(fd);

    // delete event
    std::remove_if(this->_cache.begin(), this->_cache.end(), [=] (::pollfd &item) {
        return item.fd == fd;
    });

    // repoll if in polling
    this->_repoll.set();
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::interrupted); code = this->poll())
        ;
}

std::error_code chen::reactor::poll(double timeout)
{
    // poll events
    std::vector<::pollfd> cache;
    int result = 0;

    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            cache = this->_cache;  // avoid race condition
        }

        // reset repoll event
        this->_repoll.reset();

        result = ::WSAPoll(cache.data(), cache.size(), timeout < 0 ? -1 : static_cast<int>(timeout * 1000));

        // repoll if user call set or del when polling
        bool repoll = false;

        if (result == 1)
        {
            for (auto it = cache.begin(); it != cache.end(); ++it)
            {
                if (it->revents && (it->fd == this->_repoll.native()))
                {
                    repoll = true;
                    break;
                }
            }
        }

        if (!repoll)
            break;
    }

    if (result <= 0)
    {
        if (!result)
            return std::make_error_code(std::errc::timed_out);  // timeout if result is zero
        else
            throw std::system_error(sys::error(), "reactor: failed to poll event");
    }

    // events on the same fd will be notified only once
    for (auto it = cache.begin(); it != cache.end(); ++it)
    {
        auto &item = *it;

        // continue if revents is 0
        if (!item.revents)
            continue;

        // user request to stop
        if (item.fd == this->_wakeup.native())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        // invoke callback
        callback func;
        int flag;

        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            func = chen::map::find(this->_calls, item.fd);
            flag = chen::map::find(this->_flags, item.fd);
        }

        if (flag & FlagOnce)
            this->del(item.fd);

        if (func)
            func(this->type(item.revents));
    }

    return {};
}

void chen::reactor::stop()
{
    // notify wakeup message via socket
    this->_wakeup.set();
}

// misc
int chen::reactor::type(int events)
{
    // check events, multiple events maybe occur
    if ((events & POLLERR) || (events & POLLHUP))
    {
        return Closed;
    }
    else
    {
        int ret = 0;

        if (events & POLLIN)
            ret |= Readable;

        if (events & POLLOUT)
            ret |= Writable;

        return ret;
    }
}

#endif