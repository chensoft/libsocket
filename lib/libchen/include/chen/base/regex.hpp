/**
 * Created by Jian Chen
 * @since   2016.04.29
 * @author  Jian Chen <admin@chensoft.com>
 * @link    http://chensoft.com
 * @caution regex is incomplete in some compilers, like gcc 4.8.x
 */
#pragma once

#include <string>
#include <vector>
#include <regex>

namespace chen
{
    class regex
    {
    public:
        /**
         * Match
         */
        static bool match(const std::string &text, const std::string &pattern);
        static bool match(const std::string &text, const std::regex &regex);

        /**
         * Replace
         */
        static std::string replace(const std::string &text, const std::string &pattern, const std::string &replace);
        static std::string replace(const std::string &text, const std::regex &regex, const std::string &replace);

        /**
         * Expand
         * match text first, then use submatch to expand the final string
         */
        static std::string expand(const std::string &text, const std::string &pattern, const std::string &expand);
        static std::string expand(const std::string &text, const std::regex &regex, const std::string &expand);

        /**
         * Group catch
         */
        static std::vector<std::string> group(const std::string &text, const std::string &pattern);
        static std::vector<std::string> group(const std::string &text, const std::regex &regex);
    };
}