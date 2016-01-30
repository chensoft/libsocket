/**
 * Created by Jian Chen
 * @since  2016.01.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "path.hpp"
#include "log.hpp"

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
    // todo handle windows
    if (path.empty())
        return "";

    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;

    auto flag = false;

    for (; idx != end; ++idx)
    {
        if ((*idx == '/') || (*idx == '\\'))
        {
            auto next = idx + 1;

            if (flag && (*next != '/') && (*next != '\\'))
            {
                ++idx;
                break;
            }
        }
        else if (!flag)
        {
            flag = true;
        }
    }

    if (flag)
    {
        if (idx != end)
        {
            return path.substr(0, static_cast<std::size_t>(end - idx));
        }
        else
        {
            auto first = path[0];
            return (first == '/') || (first == '\\') ? "/" : ".";  // using root folder or current folder
        }
    }
    else
    {
        return "/";  // all chars are '/'
    }
}

std::string path::basename(const std::string &path)
{
    auto beg = path.rbegin();
    auto end = path.rend();

    auto flag_a = end;  // before
    auto flag_b = end;  // after

    for (auto idx = beg; idx != end; ++idx)
    {
        if ((*idx == '/') || (*idx == '\\'))
        {
            if (flag_b != end)
            {
                flag_a = idx;
                break;
            }
        }
        else if (flag_b == end)
        {
            flag_b = idx;
        }
    }

    if ((flag_a != end) && (flag_b != end))
        return path.substr(static_cast<std::size_t>(end - flag_a), static_cast<std::size_t>(flag_a - flag_b));
    else
        return "";
}

std::string path::extname(const std::string &path, std::size_t dots)
{
    if (!dots)
        return "";

    auto beg = path.rbegin();
    auto end = path.rend();
    auto idx = beg;
    auto cur = beg;
    auto num = 0;

    for (; idx != end; ++idx)
    {
        if ((*idx == '/') || (*idx == '\\'))
        {
            break;
        }
        else if (*idx == '.')
        {
            cur = idx + 1;

            if (++num == dots)
                break;
        }
    }

    return num ? path.substr(static_cast<std::size_t>(end - cur), static_cast<std::size_t>(cur - beg)) : "";
}