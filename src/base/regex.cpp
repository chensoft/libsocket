/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/base/regex.hpp>
#include <regex>

using namespace chen;

// -----------------------------------------------------------------------------
// regex
bool regex::match(const std::string &text, const std::string &pattern)
{
    return std::regex_search(text, std::regex(pattern));
}

std::string regex::replace(const std::string &text,
                           const std::string &pattern,
                           const std::string &replace)
{
    return std::regex_replace(text, std::regex(pattern), replace);
}

std::vector<std::string> regex::group(const std::string &text,
                                      const std::string &pattern)
{
    std::vector<std::string> ret;

    std::smatch mt;

    if (std::regex_search(text, mt, std::regex(pattern)))
    {
        for (auto &str : mt)
            ret.emplace_back(str.str());
    }

    return ret;
}