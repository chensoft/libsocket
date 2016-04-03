/**
 * Created by Jian Chen
 * @since  2015.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>
#include <ctime>

namespace chen
{
    class date
    {
    public:
        /**
         * GMT & Local time, thread safe
         */
        static struct tm gmtime(std::time_t time);
        static struct tm localtime(std::time_t time);

        /**
         * Date stamp
         */
        static std::string stamp(const std::string &sep = "-", bool utc = true);

        /**
         * Time string
         */
        static std::string time(const std::string &sep = ":", bool utc = true, bool microseconds = false);
    };
}