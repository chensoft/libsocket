/**
 * Created by Jian Chen
 * @since  2015.07.29
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <string>
#include <vector>

namespace chen
{
    class str
    {
    public:
        // todo add more string algorithms
        /**
         * Format string using std::vsnprintf
         */
        static std::string format(const char *fmt, ...);

        /**
         * Test if two strings are equal
         */
        static bool equal(const char *str1, std::size_t size1,
                          const char *str2, std::size_t size2);

        /**
         * Regex match
         */
        static bool match(const std::string &pattern, const std::string &text);

        /**
         * Regex replace
         */
        static std::string replace(const std::string &pattern,
                                   const std::string &replacement,
                                   const std::string &text);

        /**
         * Regex group catch
         */
        static std::vector<std::string> group(const std::string &pattern, const std::string &text);

        /**
         * Levenshtein distance
         * @link https://en.wikipedia.org/wiki/Levenshtein_distance
         */
        static std::size_t levenshtein(const char *s, std::size_t len_s, const char *t, std::size_t len_t);
    };
}