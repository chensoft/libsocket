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
//    auto beg  = path.rbegin();
//    auto end  = path.rend();
//    bool flag = false;
//
//    for (auto it = beg; it != end; ++it)
//    {
//        if ((*it == '/') || (*it == '\\'))
//        {
//            if (flag)
//            {
//                auto len = end - it;
//                if (len > 1)
//                    return path.substr(0, static_cast<std::size_t>(len) - 1);
//            }
//        }
//        else
//        {
//            flag = true;
//        }
//    }
//
//    return path.empty() ? "" : (path[0] == '/' ? "/" : ".");
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