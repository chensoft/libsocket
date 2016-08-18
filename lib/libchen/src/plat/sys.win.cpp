/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <chen/sys/sys.hpp>
#include <Windows.h>

// -----------------------------------------------------------------------------
// sys
std::string chen::sys::error()
{
    char buf[1024] = {};
    return !errno ? "No error" : (!::strerror_s(buf, sizeof(buf), errno) ? std::string(buf) : "Unknown error");
}

#endif