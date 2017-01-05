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
    this->_fd = ::eventfd(0, 0);

    if ((this->_fd < 0) || ::fcntl(this->_fd, F_SETFL, ::fcntl(this->_fd, F_GETFL, 0) | O_NONBLOCK))
    {
        ::close(this->_fd);
        throw std::system_error(sys::error(), "event: failed to create eventfd");
    }
}

chen::event::~event()
{
    ::close(this->_fd);
}

void chen::event::set()
{
    ::eventfd_write(this->_fd, 1);
}

void chen::event::reset()
{
    ::eventfd_t dummy;
    ::eventfd_read(this->_fd, &dummy);
}

chen::handle_t chen::event::native() const
{
    return this->_fd;
}

#endif