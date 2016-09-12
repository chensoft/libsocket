/**
 * Created by Jian Chen
 * @since  2015.12.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <stdexcept>
#include <string>

namespace chen
{
    namespace dns
    {
        /**
         * General error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : runtime_error(what) {}
        };


        /**
         * Codec error
         */
        class codec_error : public error
        {
        public:
            explicit codec_error(const std::string &what) : error(what) {}
        };


        /**
         * Fqdn error
         */
        class fqdn_error : public error
        {
        public:
            explicit fqdn_error(const std::string &what) : error(what) {}
        };
    }
}