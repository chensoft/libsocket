/**
 * Created by Jian Chen
 * @since  2016.07.11
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <stdexcept>
#include <string>

namespace chen
{
    namespace ip
    {
        /**
         * IP general error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };


        /**
         * Convert error
         */
        class error_convert : public error
        {
        public:
            explicit error_convert(const std::string &what) : error(what) {}
        };
    }
}