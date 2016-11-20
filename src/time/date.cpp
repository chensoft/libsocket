/**
 * Created by Jian Chen
 * @since  2015.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/time/date.hpp>
#include <chen/base/str.hpp>
#include <chrono>

// -----------------------------------------------------------------------------
// date
std::string chen::date::stamp(const std::string &sep, bool utc)
{
    auto time = std::time(nullptr);
    auto  now = utc ? date::gmtime(time) : date::localtime(time);

    auto year  = now.tm_year + 1900;
    auto month = now.tm_mon  + 1;
    auto day   = now.tm_mday;

    return str::format("%d%s%02d%s%02d", year, sep.c_str(), month, sep.c_str(), day);
}

std::string chen::date::time(const std::string &sep, bool utc, bool microseconds)
{
    auto high = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto last = high - high / 1000000 * 1000000;

    auto time = (time_t)(high / 1000000);
    auto  now = utc ? date::gmtime(time) : date::localtime(time);

    auto hour   = now.tm_hour;
    auto minute = now.tm_min;
    auto second = now.tm_sec;

    if (microseconds)
        return str::format("%02d%s%02d%s%02d.%06lld", hour, sep.c_str(), minute, sep.c_str(), second, last);
    else
        return str::format("%02d%s%02d%s%02d", hour, sep.c_str(), minute, sep.c_str(), second);
}