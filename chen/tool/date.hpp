/**
 * Created by Jian Chen
 * @since  2015.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <ctime>

namespace chen
{
    class date
    {
    public:
        /**
         * Thread safe functions
         */
        static struct tm gmtime(std::time_t time);
        static struct tm localtime(std::time_t time);
    };
}