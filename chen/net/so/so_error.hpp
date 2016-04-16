/**
 * Created by Jian Chen
 * @since  2015.08.23
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <stdexcept>
#include <string>

namespace chen
{
    namespace so
    {
        /**
         * Socket general error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what) : std::runtime_error(what) {}
        };


        /**
         * Socket build error
         */
        class error_build : public error
        {
        public:
            explicit error_build(const std::string &what) : error(what) {}
        };


        /**
         * Socket bind error
         */
        class error_bind : public error
        {
        public:
            explicit error_bind(const std::string &what) : error(what) {}
        };


        /**
         * Socket listen error
         */
        class error_listen : public error
        {
        public:
            explicit error_listen(const std::string &what) : error(what) {}
        };


        /**
         * Socket accept error
         */
        class error_accept : public error
        {
        public:
            explicit error_accept(const std::string &what) : error(what) {}
        };


        /**
         * Socket send error
         */
        class error_send : public error
        {
        public:
            explicit error_send(const std::string &what) : error(what) {}
        };


        /**
         * Socket recv error
         */
        class error_recv : public error
        {
        public:
            explicit error_recv(const std::string &what) : error(what) {}
        };
    }
}