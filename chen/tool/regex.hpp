/**
 * Created by Jian Chen
 * @since  2016.04.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
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
        static bool match(const std::string &pattern, const std::string &text);

        /**
         * Replace
         */
        static std::string replace(const std::string &pattern,
                                   const std::string &replacement,
                                   const std::string &text);

        /**
         * Group catch
         */
        static std::vector<std::string> group(const std::string &pattern, const std::string &text);
    };
}