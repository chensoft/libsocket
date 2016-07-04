/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/sys/sys.hpp>
#include <string.h>
#include <cerrno>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::error()
{
    char buf[1024] = {0};
    return errno && !::strerror_r(errno, buf, sizeof(buf)) ? std::string(buf) : "No error";
}