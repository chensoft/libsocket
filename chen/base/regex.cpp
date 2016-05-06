/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include "regex.hpp"
#include <regex>

using namespace chen;

// -----------------------------------------------------------------------------
// regex
bool regex::match(const std::string &pattern, const std::string &text)
{
    return std::regex_search(text, std::regex(pattern));
}

std::string regex::replace(const std::string &pattern,
                           const std::string &replacement,
                           const std::string &text)
{
    return std::regex_replace(text, std::regex(pattern), replacement);
}

std::vector<std::string> regex::group(const std::string &pattern,
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