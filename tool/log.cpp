/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "log.h"
#include <iostream>

using namespace chen;

// -----------------------------------------------------------------------------
// log
#pragma mark -
#pragma mark log

log::log()
{

}

log::~log()
{

}

log& log::standard()
{
    static log instance;
    return instance;
}

#pragma mark -
#pragma mark Trace

void log::debug(const std::string &text)
{
    this->flush("[DEBUG] " + text);
}

void log::error(const std::string &text)
{
    this->flush("[ERROR] " + text);
}

void log::fatal(const std::string &text)
{
    this->flush("[FATAL] " + text);
    exit(1);
}

void log::flush(const std::string &text)
{
    std::cout << text << std::endl;
}

#pragma mark -
#pragma mark Tool

std::string log::date(const std::string &sep, bool gmt)
{
    std::time_t time = std::time(nullptr);
    struct tm   *now = gmt ? std::gmtime(&time) : std::localtime(&time);

    auto year  = now->tm_year + 1900;
    auto month = now->tm_mon  + 1;
    auto day   = now->tm_mday;

    return log::format("%d%s%02d%s%02d", year, sep.c_str(), month, sep.c_str(), day);
}

std::string log::time(const std::string &sep, bool gmt, bool milliseconds)
{
    auto high = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto last = high - high / 1000 * 1000;

    std::time_t time = (time_t)(high / 1000);
    struct tm   *now = gmt ? std::gmtime(&time) : std::localtime(&time);

    auto hour   = now->tm_hour;
    auto minute = now->tm_min;
    auto second = now->tm_sec;

    if (milliseconds)
        return log::format("%02d%s%02d%s%02d.%03lld", hour, sep.c_str(), minute, sep.c_str(), second, last);
    else
        return log::format("%02d%s%02d%s%02d", hour, sep.c_str(), minute, sep.c_str(), second);
}

std::string log::format(const char *fmt, ...)
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