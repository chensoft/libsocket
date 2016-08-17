/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <chen/sys/sys.hpp>
#include <string.h>
#include <cstdlib>
#include <cerrno>

// -----------------------------------------------------------------------------
// helper
namespace
{
    // disable unused function warning on OS X
#ifndef __APPLE__
    std::string peek(char *result, char *buffer)
    {
        // GNU-specific strerror_r, result type is char*, will not write to buffer
        return result;
    }
#endif

    std::string peek(int result, char *buffer)
    {
        // XSI-compliant strerror_r, result type is int, write error string to buffer
        return !result ? buffer : "Unknown error";
    }
}


// -----------------------------------------------------------------------------
// sys
std::string chen::sys::error()
{
    char buf[1024] = {0};
    return !errno ? "No error" : peek(::strerror_r(errno, buf, sizeof(buf)), buf);
}

#endif