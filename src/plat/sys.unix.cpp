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

#ifndef ANDROID
#include <execinfo.h>
#endif

using namespace chen;

// -----------------------------------------------------------------------------
// helper
namespace
{
    std::string peek(char *result, char *buffer)
    {
        // GNU-specific strerror_r, result type is char*, will not write to buffer
        return result;
    }
    
    std::string peek(int result, char *buffer)
    {
        // XSI-compliant strerror_r, result type is int, write error string to buffer
        return !result ? buffer : "Unknown error";
    }
}


// -----------------------------------------------------------------------------
// sys
std::string sys::error()
{
    char buf[1024] = {0};
    return !errno ? "No error" : peek(::strerror_r(errno, buf, sizeof(buf)), buf);
}

#ifndef ANDROID
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

#endif