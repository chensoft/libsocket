/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#pragma once

#include <stdexcept>
#include <string>

namespace chen
{
    namespace dns
    {
        /**
         * Base class for dns related error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what);
        };


        /**
         * Fqdn create error
         */
        class error_fqdn : public error
        {
        public:
            explicit error_fqdn(const std::string &what);
        };


        /**
         * Size error
         */
        class error_size : public error
        {
        public:
            explicit error_size(const std::string &what);
        };


        /**
         * Nameserver error
         */
        class error_nameserver : public error
        {
        public:
            explicit error_nameserver(const std::string &what);
        };
    }
}