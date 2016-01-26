/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "str.hpp"
#include <cstdarg>
#include <regex>

using namespace chen;

// -----------------------------------------------------------------------------
// str
bool str::match(const std::string &pattern, const std::string &text)
{
    return std::regex_search(text, std::regex(pattern));
}

std::string str::replace(const std::string &pattern,
                         const std::string &replacement,
                         const std::string &text)
{
    return std::regex_replace(text, std::regex(pattern), replacement);
}

std::vector<std::string> str::group(const std::string &pattern,
                                      const std::string &text)
{
    std::vector<std::string> ret;

    std::smatch mt;

    if (std::regex_search(text, mt, std::regex(pattern)))
    {
        for (auto &str : mt)
        {
            ret.push_back(str.str());
        }
    }

    return ret;
}

std::string str::format(const char *fmt, ...)
{
    va_list v1, v2;

    va_start(v1, fmt);
    va_copy(v2, v1);

    auto tmp = std::vsnprintf(nullptr, 0, fmt, v1) + 1;

    if (tmp <= 0)
        return "";

    auto len = static_cast<std::size_t>(tmp);
    auto buf = new char[len];

    std::vsnprintf(buf, len, fmt, v2);
    std::string ret(buf);

    delete[] buf;

    va_end(v1);
    va_end(v2);

    return ret;
}