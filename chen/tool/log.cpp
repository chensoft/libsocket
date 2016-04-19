/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "log.hpp"
#include "date.hpp"
#include <iostream>

using namespace chen;

// -----------------------------------------------------------------------------
// log
std::mutex log::_mutex;

chen::log& log::standard()
{
    static log inst;
    return inst;
}

void log::limit(Level level)
{
    this->_level = level;
}

log::Level log::level() const
{
    return this->_level;
}

void log::output(const std::string &text, chen::log::Level level)
{
    std::lock_guard<std::mutex> lock(log::_mutex);

    if (!this->_redirect)
    {
        std::cout << chen::date::stamp() << " " << chen::date::time(":", true, true) << " UTC [";

        switch (level)
        {
            case Level::Trace:
                std::cout << "T";
                break;

            case Level::Debug:
                std::cout << "D";
                break;

            case Level::Info:
                std::cout << "I";
                break;

            case Level::Warn:
                std::cout << "W";
                break;

            case Level::Error:
                std::cout << "E";
                break;

            case Level::Fatal:
                std::cout << "F";
                break;
        }

        std::cout << "] " << text << std::endl;
    }
    else
    {
        this->_redirect(text, level);
    }
}

void log::redirect(std::function<void (const std::string &text, chen::log::Level level)> callback)
{
    this->_redirect = callback;
}