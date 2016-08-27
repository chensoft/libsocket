/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <chen/sys/sys.hpp>
#include <cstring>
#include <cstdlib>
#include <cerrno>

// -----------------------------------------------------------------------------
// sys
std::string chen::sys::error()
{
    char buf[1024] = {};
    return !errno ? "No error" : sys::peek(::strerror_r(errno, buf, sizeof(buf)), buf);
}

#endif