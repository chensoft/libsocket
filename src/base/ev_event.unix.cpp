/**
 * Created by Jian Chen
 * @since  2017.02.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

#include "socket/base/ev_event.hpp"
#include "socket/core/reactor.hpp"
#include "socket/core/ioctl.hpp"
#include "chen/sys/sys.hpp"
#include <unistd.h>

// -----------------------------------------------------------------------------
// ev_event_impl
namespace chen
{
    struct ev_event_impl
    {
        handle_t write;
    };
}


// -----------------------------------------------------------------------------
// ev_event
chen::ev_event::ev_event(std::function<void ()> cb) : _notify(std::move(cb)), _impl(new ev_event_impl)
{
    handle_t pp[2]{};

    if (::pipe(pp) < 0)
        throw std::system_error(sys::error(), "event: failed to create pipe");

    if (ioctl::nonblocking(pp[0], true) || ioctl::nonblocking(pp[1], true))
    {
        ::close(pp[0]);
        ::close(pp[1]);
        throw std::system_error(sys::error(), "event: failed to create pipe");
    }

    ioctl::cloexec(pp[0], true);
    ioctl::cloexec(pp[1], true);

    this->change(pp[0]);   // read
    this->_impl->write = pp[1];  // write
}

chen::ev_event::~ev_event()
{
    ::close(this->_impl->write);
}

void chen::ev_event::set()
{
    this->_signaled = true;
    ::write(this->_impl->write, "\n", 1);
}

void chen::ev_event::reset()
{
    char buf[512];
    while (::read(this->native(), buf, 512) >= 0)
        ;

    this->_signaled = false;
}

// notify
void chen::ev_event::attach(std::function<void ()> cb)
{
    this->_notify = std::move(cb);
}

// event
void chen::ev_event::onEvent(int type)
{
    auto loop = this->evLoop();
    auto func = this->_notify;

    if (loop && ((type & Closed) || (this->evFlag() & reactor::FlagOnce)))
        loop->del(this);

    if (func)
        func();
}

#endif