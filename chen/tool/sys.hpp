/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>

namespace chen
{
    class sys
    {
    public:
        /**
         * Date string
         */
        static std::string date(const std::string &sep = "-", bool utc = true);

        /**
         * Time string
         */
        static std::string time(const std::string &sep = ":", bool utc = true, bool milliseconds = false);

        /**
         * UUID string
         */
        static std::string uuid();
    };
}