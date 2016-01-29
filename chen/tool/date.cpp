/**
 * Created by Jian Chen
 * @since  2015.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "date.hpp"
#include "str.hpp"
#include <chrono>

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
    #error system not supported
#endif

    return now;
}

struct tm date::localtime(std::time_t time)
{
#if defined(CHEN_OS_WIN32)
    struct tm now;
    ::localtime_s(&now, &time);
#elif defined(CHEN_OS_UNIX)
    struct tm now;
    ::localtime_r(&time, &now);
#else
    #error system not supported
#endif

    return now;
}

std::string date::stamp(const std::string &sep, bool utc)
{
    std::time_t time = std::time(nullptr);
    struct tm    now = utc ? date::gmtime(time) : date::localtime(time);

    auto year  = now.tm_year + 1900;
    auto month = now.tm_mon  + 1;
    auto day   = now.tm_mday;

    return str::format("%d%s%02d%s%02d", year, sep.c_str(), month, sep.c_str(), day);
}

std::string date::time(const std::string &sep, bool utc, bool milliseconds)
{
    auto high = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto last = high - high / 1000 * 1000;

    std::time_t time = (time_t)(high / 1000);
    struct tm    now = utc ? date::gmtime(time) : date::localtime(time);

    auto hour   = now.tm_hour;
    auto minute = now.tm_min;
    auto second = now.tm_sec;

    if (milliseconds)
        return str::format("%02d%s%02d%s%02d.%03lld", hour, sep.c_str(), minute, sep.c_str(), second, last);
    else
        return str::format("%02d%s%02d%s%02d", hour, sep.c_str(), minute, sep.c_str(), second);
}