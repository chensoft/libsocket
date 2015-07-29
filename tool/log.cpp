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
    static log instance;
    return instance;
}

#pragma mark -
#pragma mark Trace

void log::debug(const std::string &text)
{
    this->flush("[DEBUG] " + text);
}

void log::error(const std::string &text)
{
    this->flush("[ERROR] " + text);
}

void log::fatal(const std::string &text)
{
    this->flush("[FATAL] " + text);
    exit(EXIT_FAILURE);
}

void log::flush(const std::string &text)
{
    std::cout << text << std::endl;
}