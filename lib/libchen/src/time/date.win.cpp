/**
 * Created by Jian Chen
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifdef _WIN32

#include <chen/time/date.hpp>

// -----------------------------------------------------------------------------
// date
struct tm chen::date::gmtime(std::time_t time)
{
    struct tm now = {};
    ::gmtime_s(&now, &time);
    return now;
}

struct tm chen::date::localtime(std::time_t time)
{
    struct tm now = {};
    ::localtime_s(&now, &time);
    return now;
}

#endif