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
std::mutex log::_mutex;

chen::log& log::standard()
{
    static log inst;
    return inst;
}

log::log()
: _stream(&std::cout)
{

}

log::log(log &&o)
{
    *this = std::move(o);
}

log& log::operator=(log &&o)
{
    if (this == &o)
        return *this;

    std::lock_guard<std::mutex> lock(log::_mutex);

    this->_level    = o._level;
    this->_stream   = o._stream;
    this->_hook     = o._hook;
    this->_redirect = std::move(o._redirect);

    o._level    = chen::log::Level::Trace;
    o._stream   = nullptr;
    o._hook     = nullptr;
    o._redirect = nullptr;

    return *this;
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
    std::lock_guard<std::mutex> lock(log::_mutex);
    this->_hook = callback;
}

void log::redirect(std::unique_ptr<std::ostream> &&stream)
{
    std::lock_guard<std::mutex> lock(log::_mutex);
    this->_redirect = std::move(stream);
    this->_stream   = this->_redirect ? this->_redirect.get() : &std::cout;
}

void log::output(const std::string &text, chen::log::Level level)
{
    std::lock_guard<std::mutex> lock(log::_mutex);

    if (!this->_hook)
    {
        (*this->_stream) << chen::date::stamp() << " " << chen::date::time(":", true, true) << " UTC [";

        static std::map<chen::log::Level, std::string> map = {
                {Level::Trace, "T"},
                {Level::Debug, "D"},
                {Level::Info,  "I"},
                {Level::Warn,  "W"},
                {Level::Error, "E"},
                {Level::Fatal, "F"},
        };

        (*this->_stream) << map[level] << "] " << text << std::endl;
    }
    else
    {
        this->_hook(text, level);
    }
}