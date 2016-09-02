/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/base/base_error.hpp>

namespace chen
{
    namespace dns
    {
        /**
         * Codec error
         */
        class error_codec : public error
        {
        public:
            explicit error_codec(const std::string &what) : error(what) {}
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