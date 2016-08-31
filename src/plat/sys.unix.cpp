/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <chen/sys/sys.hpp>
#include <cerrno>

// -----------------------------------------------------------------------------
// sys
std::error_code chen::sys::error()
{
    return std::error_code(errno, std::system_category());
}

#endif