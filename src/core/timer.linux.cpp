/**
 * Created by Jian Chen
 * @since  2017.01.15
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef __linux__

#include <socket/core/timer.hpp>
#include <chen/sys/sys.hpp>
#include <sys/timerfd.h>

// -----------------------------------------------------------------------------
// timer
chen::timer::timer()
{
    auto fd = ::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
    if (fd < 0)
        throw std::system_error(sys::error(), "timer: failed to create timerfd");

    this->_handle.change(fd);
}

chen::timer::~timer()
{
}

#endif