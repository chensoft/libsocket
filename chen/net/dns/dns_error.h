/**
 * Created by Jian Chen
 * @since  2015.12.02
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
         * DNS general error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };


        /**
         * Size error
         */
        class error_size : public error
        {
        public:
            explicit error_size(const std::string &what) : error(what) {}
        };


        /**
         * Fqdn error
         */
        class error_fqdn : public error
        {
        public:
            explicit error_fqdn(const std::string &what) : error(what) {}
        };


        /**
         * Nameserver error
         */
        class error_nameserver : public error
        {
        public:
            explicit error_nameserver(const std::string &what) : error(what) {}
        };
    }
}