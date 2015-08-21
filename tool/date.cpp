/**
 * Created by Jian Chen
 * @since  2015.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "date.h"

using namespace chen;

// -----------------------------------------------------------------------------
// date
struct tm date::gmtime(std::time_t time)
{
#if defined(CHEN_OS_WIN32)
    struct tm now;
    ::gmtime_s(&now, &time);
#elif defined(CHEN_OS_UNIX)
    struct tm now;
    ::gmtime_r(&time, &now);
#else
    // system not supported
#endif

    return now;
}

struct tm date::localtime(std::time_t time)
{
#if defined(CHEN_OS_WIN32)
    struct tm now;
    localtime_s(&now, &time);
#elif defined(CHEN_OS_UNIX)
    struct tm now;
    localtime_r(&time, &now);
#else
    // system not supported
#endif

    return now;
}