/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "log.hpp"
#include "date.hpp"
#include <iostream>
#include <map>

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

void log::hook(std::function<void (const std::string &text, chen::log::Level level)> callback)
{
    this->_hook = callback;
}

void log::output(const std::string &text, chen::log::Level level)
{
    std::lock_guard<std::mutex> lock(log::_mutex);

    if (!this->_hook)
    {
        std::cout << chen::date::stamp() << " " << chen::date::time(":", true, true) << " UTC [";

        static std::map<chen::log::Level, std::string> map = {
                {Level::Trace, "T"},
                {Level::Debug, "D"},
                {Level::Info,  "I"},
                {Level::Warn,  "W"},
                {Level::Error, "E"},
                {Level::Fatal, "F"},
        };

        std::cout << map[level] << "] " << text << std::endl;
    }
    else
    {
        this->_hook(text, level);
    }
}