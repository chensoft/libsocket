/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/str.hpp>
#include <functional>
#include <algorithm>
#include <iostream>
#include <cstdarg>
#include <memory>

using namespace chen;

// -----------------------------------------------------------------------------
// str
std::string str::format(const char *fmt, ...)
{
    va_list v1, v2;

    va_start(v1, fmt);
    va_copy(v2, v1);

    auto len = ::vsnprintf(nullptr, 0, fmt, v1);
    if (len <= 0)
        return "";

    std::string ret(static_cast<std::size_t>(len + 1), '\0');
    ::vsnprintf(&ret[0], static_cast<std::size_t>(len + 1), fmt, v2);
    ret.erase(ret.begin() + len);  // remove the redundant '\0'

    va_end(v1);
    va_end(v2);

    return ret;
}

void str::print(const std::string &text, bool br)
{
    str::print(std::cout, text, br);
}

void str::print(std::ostream &out, const std::string &text, bool br)
{
    static std::mutex mutex;
    str::print(out, mutex, text, br);
}

void str::print(std::ostream &out, std::mutex &mutex, const std::string &text, bool br)
{
    std::lock_guard<std::mutex> lock(mutex);
    br ? out << text << std::endl : out << text;
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

// prefix & suffix
bool str::hasPrefix(const std::string &text, const std::string &prefix)
{
    std::size_t size_t = text.size();
    std::size_t size_p = prefix.size();

    if (size_t < size_p)
        return false;

    for (std::size_t i = 0; i < size_p; ++i)
    {
        if (text[i] != prefix[i])
            return false;
    }

    return true;
}

bool str::hasSuffix(const std::string &text, const std::string &suffix)
{
    std::size_t size_t = text.size();
    std::size_t size_s = suffix.size();

    if (size_t < size_s)
        return false;

    for (std::size_t i = 0; i < size_s; ++i)
    {
        if (text[size_t - size_s + i] != suffix[i])
            return false;
    }

    return true;
}

// contain
bool str::contain(const std::string &text, const std::string &search)
{
    return text.find(search) != std::string::npos;
}

// count
std::size_t str::count(const std::string &text, const std::string &search)
{
    std::size_t c = 0;
    std::size_t p = text.find(search);
    std::size_t l = search.size();

    while (p != std::string::npos)
    {
        ++c;
        p = text.find(search, p + l);
    }

    return c;
}

// split
std::vector<std::string> str::split(const std::string &text, const std::string &delimiter)
{
    std::vector<std::string> ret;

    std::size_t i = 0;
    std::size_t p = text.find(delimiter);
    std::size_t l = delimiter.size();

    while (p != std::string::npos)
    {
        ret.push_back(text.substr(i, p - i));

        i = p = p + l;
        p = text.find(delimiter, p);

        if (p == std::string::npos)
        {
            ret.push_back(text.substr(i));
            break;
        }
    }

    return ret;
}

// join
std::string str::join(const std::vector<std::string> &text, const std::string &delimiter)
{
    if (text.empty())
        return "";

    std::string ret;
    std::size_t len = text.size();

    for (std::size_t i = 0; i < len - 1; ++i)
    {
        ret += text[i];
        ret += delimiter;
    }

    if (len > 1)
        ret += text[len - 1];

    return ret;
}

// replace
void str::replace(std::string &text,
                  const std::string &search,
                  const std::string &replacement,
                  bool all)
{
    std::size_t p   = 0;
    std::size_t c_s = search.size();
    std::size_t c_r = replacement.size();

    while ((p = text.find(search, p)) != std::string::npos)
    {
        text.replace(p, c_s, replacement);
        p += c_r;
    }
}

void str::remove(std::string &text,
                 const std::string &search,
                 bool all)
{
    str::replace(text, search, "", all);
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