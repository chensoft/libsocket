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
#include <memory>
#include <cctype>

using namespace chen;

// -----------------------------------------------------------------------------
// str
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
bool str::prefix(const std::string &text, const std::string &prefix)
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

bool str::suffix(const std::string &text, const std::string &suffix)
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
        ret.emplace_back(text.substr(i, p - i));

        i = p = p + l;
        p = text.find(delimiter, p);

        if (p == std::string::npos)
        {
            ret.emplace_back(text.substr(i));
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
std::string& str::replace(std::string &text, const std::string &search, const std::string &replace, bool all)
{
    std::size_t p   = 0;
    std::size_t c_s = search.size();
    std::size_t c_r = replace.size();

    while ((p = text.find(search, p)) != std::string::npos)
    {
        text.replace(p, c_s, replace);
        p += c_r;

        if (!all)
            break;
    }

    return text;
}

std::string& str::remove(std::string &text, const std::string &search, bool all)
{
    return str::replace(text, search, "", all);
}

std::string str::replace(const std::string &text, const std::string &search, const std::string &replace, bool all)
{
    std::string temp(text);
    str::replace(temp, search, replace, all);
    return temp;
}

std::string str::remove(const std::string &text, const std::string &search, bool all)
{
    std::string temp(text);
    str::remove(temp, search, all);
    return temp;
}

// trim
std::string& str::trim(std::string &text)
{
    str::ltrim(text);
    str::rtrim(text);
    return text;
}

std::string& str::ltrim(std::string &text)
{
    text.erase(text.begin(), std::find_if(text.begin(), text.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return text;
}

std::string& str::rtrim(std::string &text)
{
    text.erase(std::find_if(text.rbegin(), text.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), text.end());
    return text;
}

std::string str::trim(const std::string &text)
{
    std::string temp(text);
    str::trim(temp);
    return temp;
}

std::string str::ltrim(const std::string &text)
{
    std::string temp(text);
    str::ltrim(temp);
    return temp;
}

std::string str::rtrim(const std::string &text)
{
    std::string temp(text);
    str::rtrim(temp);
    return temp;
}

// transform
std::string& str::lowercase(std::string &text)
{
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
    return text;
}

std::string& str::uppercase(std::string &text)
{
    std::transform(text.begin(), text.end(), text.begin(), ::toupper);
    return text;
}

std::string& str::capitalize(std::string &text)
{
    if (text.empty())
        return text;

    text[0] = static_cast<char>(std::toupper(text[0]));
    return text;
}

std::string str::lowercase(const std::string &text)
{
    std::string temp(text);
    str::lowercase(temp);
    return temp;
}

std::string str::uppercase(const std::string &text)
{
    std::string temp(text);
    str::uppercase(temp);
    return temp;
}

std::string str::capitalize(const std::string &text)
{
    std::string temp(text);
    str::capitalize(temp);
    return temp;
}

// levenshtein
std::size_t str::levenshtein(const char *s, std::size_t len_s, const char *t, std::size_t len_t)
{
    // copy from https://en.wikipedia.org/wiki/Levenshtein_distance
    std::size_t cost = 0;

    // base case: empty strings
    if (len_s == 0)
        return len_t;

    if (len_t == 0)
        return len_s;

    // test if last characters of the strings match
    if (s[len_s - 1] == t[len_t - 1])
        cost = 0;
    else
        cost = 1;

    // return minimum of delete char from s, delete char from t, and delete char from both
    return std::min({str::levenshtein(s, len_s - 1, t, len_t    ) + 1,
                     str::levenshtein(s, len_s    , t, len_t - 1) + 1,
                     str::levenshtein(s, len_s - 1, t, len_t - 1) + cost});
}