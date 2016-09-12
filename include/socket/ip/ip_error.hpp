/**
 * Created by Jian Chen
 * @since  2016.09.02
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
         * General error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : runtime_error(what) {}
        };


        // ---------------------------------------------------------------------
        // Syntax error
        class syntax_error : public error
        {
        public:
            explicit syntax_error(const std::string &what) : error(what) {}
        };
    }
}