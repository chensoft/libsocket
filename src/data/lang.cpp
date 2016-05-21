/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/data/lang.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// lang
lang& lang::standard()
{
    static lang inst;
    return inst;
}

std::string translate(const std::string &text)
{
    return text;
}