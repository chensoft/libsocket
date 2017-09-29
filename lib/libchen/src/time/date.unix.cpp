/**
 * Created by Jian Chen
 * @since  2016.05.14
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#ifndef _WIN32

#include "chen/time/date.hpp"
#include <sys/time.h>
#include <time.h>

// -----------------------------------------------------------------------------
// date
chen::date::date(bool utc)
{
    struct ::tm tm{};
    struct ::timeval tv{};

    ::gettimeofday(&tv, nullptr);
    utc ? ::gmtime_r(&tv.tv_sec, &tm) : ::localtime_r(&tv.tv_sec, &tm);

    this->microsecond = tv.tv_usec;
    this->second      = tm.tm_sec;
    this->minute      = tm.tm_min;
    this->hour        = tm.tm_hour;
    this->day         = tm.tm_mday;
    this->wday        = tm.tm_wday;
    this->month       = tm.tm_mon + 1;
    this->year        = tm.tm_year + 1900;
    this->zone        = tm.tm_gmtoff;
}

#endif