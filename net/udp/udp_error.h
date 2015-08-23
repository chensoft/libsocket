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
    namespace udp
    {
        /**
         * Base class for udp related error
         */
        class error : public std::runtime_error
        {
        public:
            explicit error(const std::string &what);
        };


        /**
         * Socket create error
         */
        class error_socket : public error
        {
        public:
            explicit error_socket(const std::string &what);
        };


        /**
         * Socket bind error
         */
        class error_bind : public error
        {
        public:
            explicit error_bind(const std::string &what);
        };


        /**
         * Socket send error
         */
        class error_send : public error
        {
        public:
            explicit error_send(const std::string &what);
        };


        /**
         * Socket bind error
         */
        class error_recv : public error
        {
        public:
            explicit error_recv(const std::string &what);
        };
    }
}