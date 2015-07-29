/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <string>

namespace chen
{
    class str
    {
    public:
        /**
         * Date string
         */
        static std::string date(const std::string &sep = "-", bool gmt = true);

        /**
         * Time string
         */
        static std::string time(const std::string &sep = ":", bool gmt = true, bool milliseconds = false);

        /**
         * Format string using std::vsnprintf
         */
        static std::string format(const char *fmt, ...);
    };
}