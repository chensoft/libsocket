/**
 * Created by Jian Chen
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef CHEN_OS_UNIX

#include <chen/time/date.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// date
struct tm date::gmtime(std::time_t time)
{
    struct tm now = {0};
    ::gmtime_r(&time, &now);
    return now;
}

struct tm date::localtime(std::time_t time)
{
    struct tm now = {0};
    ::localtime_r(&time, &now);
    return now;
}

#endif