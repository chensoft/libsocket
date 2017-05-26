/**
 * Created by Jian Chen
 * @since  2017.01.09
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#if defined(__unix__) || defined(__APPLE__)

#include "socket/core/ioctl.hpp"
#include "chen/sys/sys.hpp"

// -----------------------------------------------------------------------------
// ioctl
std::error_code chen::ioctl::rlimit()
{
    struct ::rlimit rl{};

    if (::getrlimit(RLIMIT_NOFILE, &rl) < 0)
        return sys::error();

#if defined(__APPLE__) && defined(OPEN_MAX)
    rl.rlim_cur = (rl.rlim_max == RLIM_INFINITY) ? OPEN_MAX : rl.rlim_max;
#else
    rl.rlim_cur = rl.rlim_max;
#endif

    return ::setrlimit(RLIMIT_NOFILE, &rl) < 0 ? sys::error() : std::error_code();
}

std::error_code chen::ioctl::cloexec(handle_t fd, bool enable)
{
    return ::ioctl(fd, enable ? FIOCLEX : FIONCLEX) < 0 ? sys::error() : std::error_code();
}

std::size_t chen::ioctl::available(handle_t fd)
{
    int len = 0;
    return ::ioctl(fd, FIONREAD, &len) < 0 ? 0u : static_cast<std::size_t>(len);
}

std::error_code chen::ioctl::nonblocking(handle_t fd, bool enable)
{
    int set = enable ? 1 : 0;
    return ::ioctl(fd, FIONBIO, &set) < 0 ? sys::error() : std::error_code();
}

#endif