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
    class socket_error : public error
    {
    public:
        explicit socket_error(const std::string &what) : error(what) {}
    };


    // ---------------------------------------------------------------------
    // Notifier related error
    class notifier_error : public error
    {
    public:
        explicit notifier_error(const std::string &what) : error(what) {}
    };
}