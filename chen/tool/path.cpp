/**
 * Created by Jian Chen
 * @since  2016.01.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "path.hpp"

using namespace chen;

// -----------------------------------------------------------------------------
// path
std::string path::separator()
{
#if defined(CHEN_OS_WIN32)
    return "\\";
#elif defined(CHEN_OS_UNIX)
    return "/";
#else
    #error system not supported
#endif
}

std::string path::dirname(const std::string &path)
{
    auto beg = path.rbegin();
    auto end = path.rend();

    for (auto it = beg; it != end; ++it)
    {
        if (((*it == '/') || (*it == '\\')) && (it != beg))
            return "";
    }

    return "";
}

std::string path::basename(const std::string &path)
{

}

std::string path::extname(const std::string &path, int dots)
{

}