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
    // ---------------------------------------------------------------------
    // General error
    class error : public std::runtime_error
    {
    public:
        explicit error(const std::string &what) : std::runtime_error(what) {}
    };


    // ---------------------------------------------------------------------
    // Socket related error
    class error_socket : public error
    {
    public:
        explicit error_socket(const std::string &what) : error(what) {}
    };


    // ---------------------------------------------------------------------
    // Notifier related error
    class error_notifier : public error
    {
    public:
        explicit error_notifier(const std::string &what) : error(what) {}
    };
}