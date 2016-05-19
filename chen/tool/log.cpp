/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "log.hpp"
#include <chen/base/date.hpp>
#include <map>

using namespace chen;

// -----------------------------------------------------------------------------
// log
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

void log::hook(std::function<void (std::string &&text)> callback)
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_hook = callback;
}

void log::output(std::string &&text, chen::log::Level level)
{
    static std::map<chen::log::Level, std::string> map = {
            {Level::Trace, "T"},
            {Level::Debug, "D"},
            {Level::Info,  "I"},
            {Level::Warn,  "W"},
            {Level::Error, "E"},
            {Level::Fatal, "F"},
    };

    std::string out(chen::date::stamp());

    out += " ";
    out += chen::date::time(":", true, true);
    out += " UTC [";
    out += map[level];
    out += "] ";
    out += text;
    out += "\n";

    if (!this->_hook)
    {
        str::print(out, false);
    }
    else
    {
        std::lock_guard<std::mutex> lock(this->_mutex);

        if (this->_hook)  // double check
            this->_hook(std::move(out));
        else
            str::print(out, false);
    }
}