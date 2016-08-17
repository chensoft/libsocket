/**
 * Created by Jian Chen
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include <chen/time/date.hpp>

// -----------------------------------------------------------------------------
// date
struct tm chen::date::gmtime(std::time_t time)
{
    struct tm now = {0};
    ::gmtime_r(&time, &now);
    return now;
}

struct tm chen::date::localtime(std::time_t time)
{
    struct tm now = {0};
    ::localtime_r(&time, &now);
    return now;
}

#endif