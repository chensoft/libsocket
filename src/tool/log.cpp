/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "chen/tool/log.hpp"
#include "chen/time/date.hpp"
#include <map>

// -----------------------------------------------------------------------------
// log
chen::log chen::log::_default;
chen::log *chen::log::_current = &chen::log::_default;

chen::log& chen::log::current()
{
    return *log::_current;
}

chen::log::~log()
{
    if (this == &log::_default)
        log::_current = nullptr;
    else
        this->demote();
}

void chen::log::limit(log::Level level)
{
    this->_level = level;
}

chen::log::Level chen::log::level() const
{
    return this->_level;
}

void chen::log::promote()
{
    log::_current = this;
}

void chen::log::demote()
{
    if (this == log::_current)
        log::_current = &log::_default;
}

std::string chen::log::format(const std::string &text, log::Level level)
{
    static std::map<log::Level, char> map = {
            {Level::Trace, 'T'},
            {Level::Debug, 'D'},
            {Level::Info,  'I'},
            {Level::Warn,  'W'},
            {Level::Error, 'E'},
            {Level::Fatal, 'F'},
    };

    date d;
    auto r = str::format("%d-%02d-%02dT%02d:%02d:%02d.%06d%c%02ld:%02ld [%c] ",
                         d.year,
                         d.month,
                         d.day,
                         d.hour,
                         d.minute,
                         d.second,
                         d.microsecond,
                         d.zone >= 0 ? '+' : '-',
                         ::labs(d.zone) / 3600,
                         ::labs(d.zone) / 60 % 60,
                         map[level]);

    return r + text;
}

void chen::log::output(const std::string &text, log::Level level)
{
    str::print(this->format(text, level));
}