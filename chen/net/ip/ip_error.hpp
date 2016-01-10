/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <stdexcept>

namespace chen
{
    namespace ip
    {
        /**
         * Base class for udp related error
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


        /**
         * Invalid error
         */
        class error_invalid : public error
        {
        public:
            explicit error_invalid(const std::string &what) : error(what) {}
        };
    }
}