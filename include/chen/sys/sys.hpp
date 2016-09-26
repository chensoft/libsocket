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
         * System error to error_code
         * @thread-safe
		 * @notice errno on Unix, GetLastError() on Windows
         */
        static std::error_code error();

        /**
         * Custom system error code
         * e.g: you can specify errno on Windows
         */
        static std::error_code error(int code);

        /**
         * UUID string
         */
        static std::string uuid();
    };
}