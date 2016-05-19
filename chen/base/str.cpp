/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "str.hpp"
#include <functional>
#include <algorithm>
#include <iostream>
#include <cstdarg>
#include <memory>
#include <mutex>

using namespace chen;

// -----------------------------------------------------------------------------
// str
std::string str::format(const char *fmt, ...)
{
    va_list v1, v2;

    va_start(v1, fmt);
    va_copy(v2, v1);

    auto len = std::vsnprintf(nullptr, 0, fmt, v1);
    if (len <= 0)
        return "";

    std::string ret(static_cast<std::size_t>(len + 1), '\0');
    std::vsnprintf(&ret[0], ret.size(), fmt, v2);
    ret.erase(ret.begin() + len);  // remove the redundant '\0'

    va_end(v1);
    va_end(v2);

    return ret;
}

void str::print(const char *text, bool end)
{
    str::print(std::cout, text, end);
}

void str::print(const std::string &text, bool end)
{
    str::print(std::cout, text.c_str(), end);
}

void str::print(std::ostream &out, const char *text, bool end)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    end ? out << text << std::endl : out << text;
}

void str::print(std::ostream &out, const std::string &text, bool end)
{
    str::print(out, text.c_str(), end);
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

// trim
void str::trim(std::string &text)
{
    str::ltrim(text);
    str::rtrim(text);
}

void str::ltrim(std::string &text)
{
    text.erase(text.begin(), std::find_if(text.begin(), text.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void str::rtrim(std::string &text)
{
    text.erase(std::find_if(text.rbegin(), text.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), text.end());
}

// levenshtein
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