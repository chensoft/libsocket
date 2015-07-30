/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "locale.h"

using namespace chen;

// -----------------------------------------------------------------------------
// locale
#pragma mark -
#pragma mark locale

locale& locale::standard()
{
    static locale inst;
    return inst;
}

#pragma mark -
#pragma mark Translate

std::string translate(const std::string &text)
{
    return text;
}