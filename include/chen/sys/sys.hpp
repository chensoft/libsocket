/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <system_error>

namespace chen
{
    class sys
    {
    public:
        /**
         * Errno to error_code
         * @thread-safe
         */
        static std::error_code error();
        static std::error_code error(int code);

        /**
         * UUID string
         */
        static std::string uuid();
    };
}