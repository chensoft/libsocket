/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/regex.hpp>
#include <chen/base/str.hpp>
#include <chen/base/num.hpp>

using namespace chen;

// -----------------------------------------------------------------------------
// regex
bool regex::match(const std::string &text, const std::string &pattern)
{
    return regex::match(text, std::regex(pattern));
}

bool regex::match(const std::string &text, const std::regex &regex)
{
    return std::regex_search(text, regex);
}

std::string regex::replace(const std::string &text, const std::string &pattern, const std::string &replace)
{
    return regex::replace(text, std::regex(pattern), replace);
}

std::string regex::replace(const std::string &text, const std::regex &regex, const std::string &replace)
{
    return std::regex_replace(text, regex, replace);
}

std::string regex::expand(const std::string &text, const std::string &pattern, const std::string &expand)
{
    return regex::expand(text, std::regex(pattern), expand);
}

std::string regex::expand(const std::string &text, const std::regex &regex, const std::string &expand)
{
    std::smatch mt;

    auto ret = expand;
    auto idx = 0;

    if (std::regex_search(text, mt, regex))
    {
        for (auto &str : mt)
            chen::str::replace(ret, "$" + chen::num::str(idx++), str);
    }

    return ret;
}

std::vector<std::string> regex::group(const std::string &text, const std::string &pattern)
{
    return regex::group(text, std::regex(pattern));
}

std::vector<std::string> regex::group(const std::string &text, const std::regex &regex)
{
    std::vector<std::string> ret;

    std::smatch mt;

    if (std::regex_search(text, mt, regex))
    {
        for (auto &str : mt)
            ret.emplace_back(str.str());
    }

    return ret;
}