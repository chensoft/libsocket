/**
 * Created by Jian Chen
 * @since  2016.09.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <chen/sys/sys.hpp>
#include <Windows.h>

// -----------------------------------------------------------------------------
// sys
std::error_code chen::sys::error()
{
    return sys::error(::GetLastError());
}

#endif