/**
 * Created by Jian Chen
 * @since  2017.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include "socket/core/ioctl.hpp"
#include "chen/sys/sys.hpp"
#include <WinSock2.h>

// -----------------------------------------------------------------------------
// ioctl
std::error_code chen::ioctl::rlimit()
{
    // nothing to do
    return {};
}

std::error_code chen::ioctl::cloexec(handle_t fd, bool enable)
{
    // nothing to do
    return {};
}

std::size_t chen::ioctl::available(handle_t fd)
{
    u_long bytes = 0;
    return ::ioctlsocket(fd, FIONREAD, &bytes) < 0 ? 0u : static_cast<std::size_t>(bytes);
}

std::error_code chen::ioctl::nonblocking(handle_t fd, bool enable)
{
    u_long mode = enable ? 1 : 0;
    return ::ioctlsocket(fd, FIONBIO, &mode) ? sys::error() : std::error_code();
}

#endif