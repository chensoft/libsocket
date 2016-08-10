/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <chen/sys/sys.hpp>
#include <string.h>
#include <cerrno>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::error()
{
    if (!errno)
        return "No error";

    char buf[1024] = {0};

#if (!defined(_POSIX_C_SOURCE) || (_POSIX_C_SOURCE >= 200112L)) && !_GNU_SOURCE
    // XSI-compliant strerror_r
    return !::strerror_r(errno, buf, sizeof(buf)) ? std::string(buf) : "Unknown error";
#else
    // GNU-specific strerror_r
    return ::strerror_r(errno, buf, sizeof(buf));
#endif
}

#endif