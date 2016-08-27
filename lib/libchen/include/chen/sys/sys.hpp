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
         * UUID string
         */
        static std::string uuid();

    private:
        /**
         * Helper for strerror_r
         */
        static std::string peek(char *result, char *buffer);
        static std::string peek(int result, char *buffer);
    };
}