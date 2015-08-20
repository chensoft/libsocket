/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "str.h"
#include <cstdarg>
#include <chrono>
#include <ctime>

using namespace chen;

// -----------------------------------------------------------------------------
// str
std::string str::date(const std::string &sep, bool utc)
{
    std::time_t time = std::time(nullptr);

#if defined(CHEN_OS_WIN32)
    struct tm now;
    utc ? ::gmtime_s(&now, &time) : localtime_s(&now, &time);
#elif defined(CHEN_OS_UNIX)
    struct tm now;
    utc ? ::gmtime_r(&time, &now) : localtime_r(&time, &now);
#endif

    auto year  = now.tm_year + 1900;
    auto month = now.tm_mon  + 1;
    auto day   = now.tm_mday;

    return str::format("%d%s%02d%s%02d", year, sep.c_str(), month, sep.c_str(), day);
}

std::string str::time(const std::string &sep, bool utc, bool milliseconds)
{
    auto high = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto last = high - high / 1000 * 1000;

    std::time_t time = (time_t)(high / 1000);

#if defined(CHEN_OS_WIN32)
    struct tm now;
    utc ? ::gmtime_s(&now, &time) : localtime_s(&now, &time);
#elif defined(CHEN_OS_UNIX)
    struct tm now;
    utc ? ::gmtime_r(&time, &now) : localtime_r(&time, &now);
#endif

    auto hour   = now.tm_hour;
    auto minute = now.tm_min;
    auto second = now.tm_sec;

    if (milliseconds)
        return str::format("%02d%s%02d%s%02d.%03lld", hour, sep.c_str(), minute, sep.c_str(), second, last);
    else
        return str::format("%02d%s%02d%s%02d", hour, sep.c_str(), minute, sep.c_str(), second);
}

std::string str::format(const char *fmt, ...)
{
    va_list v1, v2;

    va_start(v1, fmt);
    va_copy(v2, v1);

    auto tmp = std::vsnprintf(nullptr, 0, fmt, v1) + 1;

    if (tmp <= 0)
        return "";

    auto len = static_cast<std::size_t>(tmp);
    auto buf = new char[len];

    std::vsnprintf(buf, len, fmt, v2);
    std::string ret(buf);

    delete[] buf;

    va_end(v1);
    va_end(v2);

    return ret;
}