/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <chen/sys/sys.hpp>
#include <execinfo.h>
#include <string.h>
#include <cstdlib>
#include <cerrno>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::error()
{
    char buf[1024] = {0};

#if (!defined(_POSIX_C_SOURCE) || (_POSIX_C_SOURCE >= 200112L)) && !_GNU_SOURCE
    // XSI-compliant strerror_r
    return !errno ? "No error" : (!::strerror_r(errno, buf, sizeof(buf)) ? std::string(buf) : "Unknown error");
#else
    // GNU-specific strerror_r
    return !errno ? "No error" : ::strerror_r(errno, buf, sizeof(buf));
#endif
}

std::vector<std::string> sys::stack()
{
    void *buffer[1024];

    auto size = ::backtrace(buffer, 1024);
    auto list = ::backtrace_symbols(buffer, size);

    if (!list)
        return {};

    std::vector<std::string> ret;

    for (int i = 0; i < size; ++i)
        ret.push_back(list[i]);

    ::free(list);

    return ret;
}

#endif