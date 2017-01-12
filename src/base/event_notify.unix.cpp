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
    if (::pipe(this->_pp) < 0)
        throw std::system_error(sys::error(), "event: failed to create pipe");

    if (ioctl::nonblocking(this->_pp[0], true) || ioctl::nonblocking(this->_pp[1], true))
    {
        ::close(this->_pp[0]);
        ::close(this->_pp[1]);
        throw std::system_error(sys::error(), "event: failed to create pipe");
    }

    ioctl::cloexec(this->_pp[0], true);
    ioctl::cloexec(this->_pp[1], true);
}

chen::event_notify::~event_notify()
{
    ::close(this->_pp[0]);
    ::close(this->_pp[1]);
}

void chen::event_notify::set()
{
    ::write(this->_pp[1], "\n", 1);
}

void chen::event_notify::reset()
{
    char buf[512];

    while (::read(this->_pp[0], buf, 512) >= 0)
        ;
}

chen::handle_t chen::event_notify::native() const
{
    return this->_pp[0];
}

#endif