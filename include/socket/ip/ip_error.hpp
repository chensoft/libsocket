/**
 * Created by Jian Chen
 * @since  2016.08.08
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
         * IP address error
         */
        class error_address : public error
        {
        public:
            explicit error_address(const std::string &what) : error(what) {}
        };
    }
}