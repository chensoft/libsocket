/**
 * Created by Jian Chen
 * @since  2015.07.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "log.h"
#include <iostream>

using namespace chen;

// -----------------------------------------------------------------------------
// log
#pragma mark -
#pragma mark log

std::mutex log::_mutex;

log& log::standard()
{
    static log inst;
    return inst;
}

#pragma mark -
#pragma mark Trace

void log::flush(const std::string &text)
{
    std::lock_guard<std::mutex> lock(log::_mutex);

    std::string prefix(chen::str::date() + " " + chen::str::time(":", true, true) + " UTC ");
    std::cout << prefix << text << std::endl;
}