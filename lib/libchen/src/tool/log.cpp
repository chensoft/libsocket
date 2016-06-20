/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/tool/log.hpp>
#include <chen/time/date.hpp>
#include <map>

using namespace chen;

// -----------------------------------------------------------------------------
// log
chen::log  log::_default;
chen::log *log::_current = &log::_default;

chen::log& log::current()
{
    return *log::_current;
}

log::~log()
{
    if (this == &log::_default)
        log::_current = nullptr;
    else
        this->demote();
}

void log::limit(chen::log::Level level)
{
    this->_level = level;
}

chen::log::Level log::level() const
{
    return this->_level;
}

void log::promote()
{
    log::_current = this;
}

void log::demote()
{
    if (this == log::_current)
        log::_current = &log::_default;
}

std::string log::format(const std::string &text, chen::log::Level level)
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
    
    return out;
}

void log::output(const std::string &text, chen::log::Level level)
{
    chen::str::print(this->format(text, level));
}