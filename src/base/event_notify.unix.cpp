/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

#include <socket/base/event_notify.hpp>
#include <socket/core/ioctl.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// event
chen::event_notify::event_notify()
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
    this->_write = pp[1];  // write
}

chen::event_notify::~event_notify()
{
    ::close(this->_write);
}

void chen::event_notify::set()
{
    ::write(this->_write, "\n", 1);
}

void chen::event_notify::reset()
{
    char buf[512];

    while (::read(this->native(), buf, 512) >= 0)
        ;
}

#endif