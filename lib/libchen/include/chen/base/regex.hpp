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

namespace chen
{
    class regex
    {
    public:
        /**
         * Match
         */
        static bool match(const std::string &text, const std::string &pattern);

        /**
         * Replace
         */
        static std::string replace(const std::string &text,
                                   const std::string &pattern,
                                   const std::string &replace);

        /**
         * Group catch
         */
        static std::vector<std::string> group(const std::string &text, const std::string &pattern);
    };
}