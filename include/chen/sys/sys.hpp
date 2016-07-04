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
         * Errno to string
         * @thread-safe
         */
        static std::string error();

        /**
         * UUID string
         */
        static std::string uuid();
    };
}