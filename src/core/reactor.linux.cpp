/**
 * Created by Jian Chen
 * @since  2016.12.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/reactor.hpp>
#include <chen/base/map.hpp>
#include <chen/sys/sys.hpp>
#include <limits>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

const int chen::reactor::FlagEdge = EPOLLET;
const int chen::reactor::FlagOnce = EPOLLONESHOT;

const int chen::reactor::Readable = 1 << 0;
const int chen::reactor::Writable = 1 << 1;
const int chen::reactor::Closed   = 1 << 2;

chen::reactor::reactor(std::uint8_t count) : _count(count)
{
    // create epoll file descriptor
    // 1 is just a hint, see http://man7.org/linux/man-pages/man2/epoll_create.2.html
    if ((this->_epoll = ::epoll_create(1)) < 0)
        throw std::system_error(sys::error(), "reactor: failed to create epoll");

    // create eventfd to recv wakeup message
    this->set(this->_wakeup.native(), nullptr, ModeRead, 0);
}

chen::reactor::~reactor()
{
    ::close(this->_epoll);
}

// modify
void chen::reactor::set(handle_t fd, callback cb, int mode, int flag)
{
    ::epoll_event event{};

    if (mode & ModeRead)
        event.events |= EPOLLIN;

    if (mode & ModeWrite)
        event.events |= EPOLLOUT;

    event.events |= flag | EPOLLRDHUP;
    event.data.fd = fd;

    // register event
    if (::epoll_ctl(this->_epoll, EPOLL_CTL_MOD, fd, &event) != 0)
    {
        if ((errno != ENOENT) || (::epoll_ctl(this->_epoll, EPOLL_CTL_ADD, fd, &event) != 0))
            throw std::system_error(sys::error(), "reactor: failed to set event");
    }

    // register callback
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_calls[fd] = cb;
}

void chen::reactor::del(handle_t fd)
{
    // delete callback
    {
        std::lock_guard<std::mutex> lock(this->_mutex);
        this->_calls.erase(fd);
    }

    // delete event
    if ((::epoll_ctl(this->_epoll, EPOLL_CTL_DEL, fd, nullptr) != 0) && (errno != ENOENT) && (errno != EBADF))
        throw std::system_error(sys::error(), "reactor: failed to delete event");
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::interrupted); code = this->poll())
        ;
}

std::error_code chen::reactor::poll()
{
    return this->poll(std::chrono::nanoseconds::min());
}

std::error_code chen::reactor::poll(const std::chrono::nanoseconds &timeout)
{
    // poll events
    ::epoll_event events[this->_count];  // VLA
    int result = ::epoll_wait(this->_epoll, events, this->_count, timeout < std::chrono::nanoseconds::zero() ? -1 : static_cast<int>(timeout.count() / 1000000));

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
        auto &item = events[i];

        // user request to stop
        if (item.data.fd == this->_wakeup.native())
        {
            this->_wakeup.reset();
            return std::make_error_code(std::errc::operation_canceled);
        }

        // invoke callback
        callback cb;

        {
            std::lock_guard<std::mutex> lock(this->_mutex);
            cb = chen::map::find(this->_calls, item.data.fd);
        }

        if (cb)
            cb(this->type(item.events));
    }

    return {};
}

void chen::reactor::stop()
{
    // notify wakeup message via eventfd
    this->_wakeup.set();
}

// misc
int chen::reactor::type(int events)
{
    // check events, multiple events maybe occur
    if ((events & EPOLLRDHUP) || (events & EPOLLERR) || (events & EPOLLHUP))
    {
        return Closed;
    }
    else
    {
        int ret = 0;

        if (events & EPOLLIN)
            ret |= Readable;

        if (events & EPOLLOUT)
            ret |= Writable;

        return ret;
    }
}

#endif