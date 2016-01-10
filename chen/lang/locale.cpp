/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "locale.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// locale
locale& locale::standard()
{
    static locale inst;
    return inst;
}

std::string translate(const std::string &text)
{
    return text;
}