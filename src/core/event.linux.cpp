/**
 * Created by Jian Chen
 * @since  2017.01.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/event.hpp>
#include <chen/sys/sys.hpp>
#include <sys/eventfd.h>

// -----------------------------------------------------------------------------
// event
chen::event::event()
{
    auto fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd < 0)
        throw std::system_error(sys::error(), "event: failed to create eventfd");

    this->_handle.change(fd);
}

chen::event::~event()
{
}

void chen::event::set()
{
    ::eventfd_write(this->_handle, 1);
}

void chen::event::reset()
{
    ::eventfd_t dummy;
    ::eventfd_read(this->_handle, &dummy);
}

chen::basic_handle& chen::event::handle()
{
    return this->_handle;
}

#endif