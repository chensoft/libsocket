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

bool str::equal(const char *str1, std::size_t size1,
                const char *str2, std::size_t size2)
{
    if (size1 != size2)
        return false;

    for (std::size_t i = 0; i < size1; ++i)
    {
        if (str1[i] != str2[i])
            return false;
    }

    return true;
}

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

std::size_t str::levenshtein(const char *s, std::size_t len_s, const char *t, std::size_t len_t)
{
    // copy from https://en.wikipedia.org/wiki/Levenshtein_distance
    std::size_t cost = 0;

    /* base case: empty strings */
    if (len_s == 0)
        return len_t;

    if (len_t == 0)
        return len_s;

    /* test if last characters of the strings match */
    if (s[len_s-1] == t[len_t-1])
        cost = 0;
    else
        cost = 1;

    /* return minimum of delete char from s, delete char from t, and delete char from both */
    return std::min({str::levenshtein(s, len_s - 1, t, len_t    ) + 1,
                     str::levenshtein(s, len_s    , t, len_t - 1) + 1,
                     str::levenshtein(s, len_s - 1, t, len_t - 1) + cost});
}