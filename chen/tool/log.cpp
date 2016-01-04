/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "log.hpp"
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

void log::output(const std::string &text)
{
    std::lock_guard<std::mutex> lock(log::_mutex);

    std::string prefix(chen::str::date() + " " + chen::str::time(":", true, true) + " UTC ");
    std::cout << prefix << text << std::endl;
}