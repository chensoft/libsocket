/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/sys.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::error()
{
    char buf[1024];
    return errno && !::strerror_s(errno, buf, 1024) ? std::string(buf) : "No error";
}

void sys::daemon()
{
    // todo not implement
}