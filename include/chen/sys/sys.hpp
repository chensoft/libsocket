/**
 * Created by Jian Chen
 * @since  2016.01.26
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>
#include <vector>

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
         * Get current thread's call stack
         */
        static std::vector<std::string> stack();

        /**
         * UUID string
         */
        static std::string uuid();
    };
}