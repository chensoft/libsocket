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

log::log()
{

}

log::~log()
{

}

log& log::standard()
{
    static log inst;
    return inst;
}

#pragma mark -
#pragma mark Trace

void log::flush(const std::string &text)
{
    std::string prefix(chen::str::date() + " " + chen::str::time(":", true, true) + " ");

    std::cout << prefix << text << std::endl;
}