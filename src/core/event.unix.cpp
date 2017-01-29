/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if (defined(__unix__) || defined(__APPLE__)) && !defined(__linux__)

#include <socket/core/event.hpp>
#include <socket/core/ioctl.hpp>
#include <chen/sys/sys.hpp>

// -----------------------------------------------------------------------------
// event
chen::event::event()
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

    this->_handle.change(pp[0]);  // read
    this->_write = pp[1];  // write
}

chen::event::~event()
{
    ::close(this->_write);
}

void chen::event::set()
{
    this->_signaled = true;

    ::write(this->_write, "\n", 1);
}

void chen::event::reset()
{
    char buf[512];
    while (::read(this->_handle, buf, 512) >= 0)
        ;

    this->_signaled = false;
}

chen::basic_handle& chen::event::handle()
{
    return this->_handle;
}

#endif