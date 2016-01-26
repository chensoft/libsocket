/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "sys.hpp"
#include "str.hpp"
#include "date.hpp"
#include <sstream>
#include <chrono>
#include <random>

using namespace chen;

// -----------------------------------------------------------------------------
// sys
std::string sys::date(const std::string &sep, bool utc)
{
    std::time_t time = std::time(nullptr);
    struct tm    now = utc ? date::gmtime(time) : date::localtime(time);

    auto year  = now.tm_year + 1900;
    auto month = now.tm_mon  + 1;
    auto day   = now.tm_mday;

    return str::format("%d%s%02d%s%02d", year, sep.c_str(), month, sep.c_str(), day);
}

std::string sys::time(const std::string &sep, bool utc, bool milliseconds)
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

std::string sys::uuid()
{
    std::string unique("xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx");

    std::stringstream ss;
    std::random_device rd;

    for (std::size_t i = 0, len = unique.size(); i < len; ++i)
    {
        auto c = unique[i];
        if ((c != 'x') && (c != 'y'))
            continue;

        auto r = (int)(((float)rd() / std::random_device::max()) * 16) | 0;
        auto v = c == 'x' ? r : ((r & 0x3) | 0x8);

        ss.str("");
        ss << std::hex << std::uppercase << v;

        unique[i] = ss.str()[0];
    }

    return unique;
}